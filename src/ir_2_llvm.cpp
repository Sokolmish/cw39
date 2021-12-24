#include "ir_2_llvm.hpp"
#include "utils.hpp"
#include <set>
#include <deque>
#include <fmt/core.h>

#include "llvm/ADT/APInt.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"
//#include "llvm/IR/Verifier.h"
//#include "llvm/ExecutionEngine/ExecutionEngine.h"
//#include "llvm/Support/TargetSelect.h"

using namespace llvm;

IR2LLVM::IR2LLVM(std::shared_ptr<ControlFlowGraph> cfg) : cfg(cfg) {
    doms = std::make_unique<Dominators>(cfg);

    context = new LLVMContext();
    module = new Module("top", *context);
    builder = new IRBuilder<>(*context);

    Function *dummyFunc = Function::Create(FunctionType::get(builder->getVoidTy(), false),
                                           Function::InternalLinkage, "__dummy_func", module);
    BasicBlock *dummyEntryBlock = BasicBlock::Create(*context, "dummy_block", dummyFunc);
    builder->SetInsertPoint(dummyEntryBlock);
    builder->CreateRet(nullptr);

    createTypes();
    createGlobals();
    createPrototypes();
    createFunctions();

    raw_string_ostream output(llvmIR);
    module->print(output, nullptr);

    delete builder;
    delete module;
    delete context;
}

std::string IR2LLVM::getRes() const {
    return llvmIR;
}

void IR2LLVM::createTypes() {
    for (auto const &[sId, tstruct] : cfg->structs) {
        std::vector<Type*> elements;
        for (auto const &elem : tstruct->fields)
            elements.push_back(getTypeFromIR(*elem.irType));
        auto type =
                StructType::create(*context, elements, fmt::format("struct{}", tstruct->structId));
        structTypes.emplace(tstruct->structId, type);
    }
}

void IR2LLVM::createGlobals() {
    for (auto const &[sId, str] : cfg->strings) {
        auto res = builder->CreateGlobalStringPtr(StringRef(str), fmt::format(".str{}", sId));
        strings.emplace(sId, res);
    }
    for (auto const &[gId, global] : cfg->globals) {
        auto ptrType = std::dynamic_pointer_cast<IR_TypePtr>(global.type);
        auto gType = ptrType->child;
                module->getOrInsertGlobal(global.name, getTypeFromIR(*gType));
        GlobalVariable *gVar = module->getNamedGlobal(global.name);
        gVar->setLinkage(GlobalValue::InternalLinkage);
        gVar->setAlignment(Align(8));
        gVar->setInitializer(getConstantFromIR(global.init));
        gVar->setConstant(false);

        globals.emplace(gId, gVar);
    }
}

llvm::Type* IR2LLVM::getTypeFromIR(const IR_Type &ir_type) {
    if (ir_type.type == IR_Type::DIRECT) {
        auto const &dirType = dynamic_cast<IR_TypeDirect const &>(ir_type);
        switch (dirType.spec) {
            case IR_TypeDirect::VOID:
                return builder->getVoidTy();
            case IR_TypeDirect::I8:
            case IR_TypeDirect::U8:
                return builder->getInt8Ty();
            case IR_TypeDirect::I32:
            case IR_TypeDirect::U32:
                return builder->getInt32Ty();
            case IR_TypeDirect::I64:
            case IR_TypeDirect::U64:
                return builder->getInt64Ty();
            case IR_TypeDirect::F32:
                return builder->getFloatTy();
        }
        throw;
    }
    else if (ir_type.type == IR_Type::POINTER) {
        auto const &ptrType = dynamic_cast<IR_TypePtr const &>(ir_type);
        return PointerType::getUnqual(getTypeFromIR(*ptrType.child));
    }
    else if (ir_type.type == IR_Type::ARRAY) {
        auto const &arrType = dynamic_cast<IR_TypeArray const &>(ir_type);
        return ArrayType::get(getTypeFromIR(*arrType.child), arrType.size);
    }
    else if (ir_type.type == IR_Type::TSTRUCT) {
        auto const &structType = dynamic_cast<IR_TypeStruct const &>(ir_type);
        return structTypes.at(structType.structId);
    }
    else if (ir_type.type == IR_Type::FUNCTION) {
        NOT_IMPLEMENTED("LLVM function type");
    }
    else {
        semanticError("Unknown type");
    }
}

llvm::Constant *IR2LLVM::getConstantFromIR(IRval const &val) {
    if (!val.isConstant())
        semanticError("LLVM Not a constant value");
    if (val.getType()->type != IR_Type::DIRECT)
        NOT_IMPLEMENTED("LLVM indirect type");
    auto const &dirType = dynamic_cast<IR_TypeDirect const &>(*val.getType());
    switch (dirType.spec) {
        case IR_TypeDirect::VOID:
            semanticError("LLVM Cannot create void constant");
        case IR_TypeDirect::I8:
            return builder->getInt8(val.castValTo<int8_t>());
        case IR_TypeDirect::U8:
            return builder->getInt8(val.castValTo<uint8_t>());
        case IR_TypeDirect::I32:
            return builder->getInt32(val.castValTo<int32_t>());
        case IR_TypeDirect::U32:
            return builder->getInt32(val.castValTo<uint32_t>());
        case IR_TypeDirect::I64:
            return builder->getInt64(val.castValTo<int64_t>());
        case IR_TypeDirect::U64:
            return builder->getInt64(val.castValTo<uint64_t>());
        case IR_TypeDirect::F32:
            semanticError("LLVM Cannot create float constant");
    }
    throw;
}

llvm::Value *IR2LLVM::getValue(const IRval &val) {
    switch (val.valClass) {
        case IRval::VAL: {
            auto const &dirType = dynamic_cast<IR_TypeDirect const &>(*val.getType());
            if (dirType.isInteger()) {
//                return builder->getIntN(dirType.getBytesSize() * 8, val.castValTo<uint64_t>());
                if (dirType.getBytesSize() == 1) {
                    return builder->getInt8(val.castValTo<uint8_t>());
                }
                else if (dirType.getBytesSize() == 4) {
                    return builder->getInt32(val.castValTo<uint32_t>());
                }
                else if (dirType.getBytesSize() == 8) {
                    return builder->getInt64(val.castValTo<uint64_t>());
                }
                else {
                    semanticError("Wrong integer size");
                }
            }
            else if (dirType.isFloat()) {
                NOT_IMPLEMENTED("Float constants");
            }
            else {
                semanticError("Wrong constant type");
            }
        }

        case IRval::VREG:
            return regsMap.at(val);

        case IRval::GLOBAL:
            return globals.at(val.castValTo<int>());

        case IRval::STRING:
            return strings.at(val.castValTo<int>());

        case IRval::FUN_PARAM:
            return curFunction->getArg(val.castValTo<int>());
    }
    throw;
}


void IR2LLVM::createPrototypes() {
    for (auto const &[fId, func] : cfg->prototypes) {
        auto irFuncType = std::dynamic_pointer_cast<IR_TypeFunc>(func.fullType);
        llvm::Type *retType = getTypeFromIR(*irFuncType->ret);
        std::vector<llvm::Type*> args;
        for (auto const &arg : irFuncType->args)
            args.push_back(getTypeFromIR(*arg));
        auto ftype = FunctionType::get(retType, args, irFuncType->isVariadic);
        auto prototype =
                Function::Create(ftype, Function::ExternalLinkage, func.getName(), *module);
        functions.emplace(fId, prototype);
    }
}

void IR2LLVM::createFunctions() {
    for (auto const &[fId, func]: cfg->funcs) {
        auto irFuncType = std::dynamic_pointer_cast<IR_TypeFunc>(func.fullType);
        llvm::Type *retType = getTypeFromIR(*irFuncType->ret);
        std::vector<llvm::Type *> args;
        for (auto const &arg: irFuncType->args)
            args.push_back(getTypeFromIR(*arg));
        auto ftype = FunctionType::get(retType, args, irFuncType->isVariadic);

        curFunction =
                Function::Create(ftype, Function::ExternalLinkage, func.getName(), *module);
        functions.emplace(fId, curFunction);

        int argsCntr = 0;
        for (auto &arg: curFunction->args()) {
            arg.setName(fmt::format(".arg_{}", argsCntr));
            argsCntr++;
        }

        std::deque<int> queue;
        std::set<int> visited;
        queue.push_back(func.getEntryBlockId());
        while (!queue.empty()) {
            int cur = queue.front();
            queue.pop_front();
            visited.insert(cur);
            for (int next: doms->getChildren(cur))
                queue.push_back(next);

            createBlock(cur);
        }

        // Link blocks
        for (int blockId: visited) {
            auto const &cfgBlock = cfg->block(blockId);
            IR_ExprTerminator const *term = cfgBlock.getTerminator();
            if (term->termType == IR_ExprTerminator::JUMP) {
                builder->SetInsertPoint(blocksMap.at(blockId));
                builder->CreateBr(blocksMap.at(cfgBlock.next.at(0)));
            }
            else if (term->termType == IR_ExprTerminator::BRANCH) {
                builder->SetInsertPoint(blocksMap.at(blockId));
                builder->CreateCondBr(
                        regsMap.at(*term->arg),
                        blocksMap.at(cfgBlock.next.at(0)),
                        blocksMap.at(cfgBlock.next.at(1)));
            }
        }

        // Link phis
        for (int blockId: visited) {
            auto const &cfgBlock = cfg->block(blockId);
            for (auto const &phiNode: cfgBlock.phis) {
                auto cfgPhiFunc = phiNode.body->getPhi();
                auto ph = unfilledPhis.at(*phiNode.res);
                for (auto const &[pos, arg]: cfgPhiFunc.args) {
                    ph->addIncoming(getValue(arg),
                                    blocksMap.at(cfgBlock.prev.at(pos)));
                }
            }
        }
        unfilledPhis.clear();
    }
}

void IR2LLVM::createBlock(int id) {
    auto const &cfgBlock = cfg->block(id);
    BasicBlock *cur_bb =
            BasicBlock::Create(*context, fmt::format("block_{}", id), curFunction);
    blocksMap.emplace(id, cur_bb);
    builder->SetInsertPoint(cur_bb);

    for (auto const &phiNode : cfgBlock.phis) {
        PHINode *phiFunc = builder->CreatePHI(
                getTypeFromIR(*phiNode.res->getType()),
                phiNode.body->getPhi().args.size(),
                phiNode.res->to_reg_name());
        unfilledPhis.emplace(*phiNode.res, phiFunc);
        regsMap.emplace(*phiNode.res, phiFunc);
    }

    for (auto const &node : cfgBlock.body) {
        switch (node.body->type) {
            case IR_Expr::OPERATION: {
                auto const &operExpr = node.body->getOper();
                auto dirType = std::dynamic_pointer_cast<IR_TypeDirect>(operExpr.args[0].getType());
                std::string resName = node.res.has_value() ? node.res->to_reg_name() : "";
                Value *res = nullptr;
                switch (operExpr.op) {
                    case IR_MUL:
                        if (dirType->isInteger()) {
                            res = builder->CreateMul(
                                    getValue(operExpr.args[0]),
                                    getValue(operExpr.args[1]),
                                    resName);
                        }
                        else if (dirType->isFloat()) {
                            res = builder->CreateFMul(
                                    getValue(operExpr.args[0]),
                                    getValue(operExpr.args[1]),
                                    resName);
                        }
                        else {
                            semanticError("Wrong mul types");
                        }
                        break;
                    case IR_DIV:
                        if (dirType->isInteger()) {
                            if (dirType->isSigned()) {
                                res = builder->CreateSDiv(
                                        getValue(operExpr.args[0]),
                                        getValue(operExpr.args[1]),
                                        resName);
                            }
                            else {
                                res = builder->CreateUDiv(
                                        getValue(operExpr.args[0]),
                                        getValue(operExpr.args[1]),
                                        resName);
                            }
                        }
                        else if (dirType->isFloat()) {
                            res = builder->CreateFDiv(
                                    getValue(operExpr.args[0]),
                                    getValue(operExpr.args[1]),
                                    resName);
                        }
                        else {
                            semanticError("Wrong div types");
                        }
                        break;
                    case IR_REM:
                        break;
                    case IR_ADD:
                        if (dirType->isInteger()) {
                            res = builder->CreateAdd(
                                    getValue(operExpr.args[0]),
                                    getValue(operExpr.args[1]),
                                    resName);
                        }
                        else if (dirType->isFloat()) {
                            res = builder->CreateFAdd(
                                    getValue(operExpr.args[0]),
                                    getValue(operExpr.args[1]),
                                    resName);
                        }
                        else {
                            semanticError("Wrong add types");
                        }
                        break;
                    case IR_SUB:
                        if (dirType->isInteger()) {
                            res = builder->CreateSub(
                                    getValue(operExpr.args[0]),
                                    getValue(operExpr.args[1]),
                                    resName);
                        }
                        else if (dirType->isFloat()) {
                            res = builder->CreateFSub(
                                    getValue(operExpr.args[0]),
                                    getValue(operExpr.args[1]),
                                    resName);
                        }
                        else {
                            semanticError("Wrong sub types");
                        }
                        break;
                    case IR_SHR:
                        if (dirType->isSigned()) {
                            res = builder->CreateAShr(
                                    getValue(operExpr.args[0]),
                                    getValue(operExpr.args[1]),
                                    resName);
                        }
                        else {
                            res = builder->CreateLShr(
                                    getValue(operExpr.args[0]),
                                    getValue(operExpr.args[1]),
                                    resName);
                        }
                        break;
                    case IR_SHL:
                        res = builder->CreateShl(
                                getValue(operExpr.args[0]),
                                getValue(operExpr.args[1]),
                                resName);
                        break;
                    case IR_XOR:
                        res = builder->CreateXor(
                                getValue(operExpr.args[0]),
                                getValue(operExpr.args[1]),
                                resName);
                        break;
                    case IR_AND:
                        res = builder->CreateAnd(
                                getValue(operExpr.args[0]),
                                getValue(operExpr.args[1]),
                                resName);
                        break;
                    case IR_OR:
                        res = builder->CreateOr(
                                getValue(operExpr.args[0]),
                                getValue(operExpr.args[1]),
                                resName);
                        break;
                    case IR_LAND:
                        res = builder->CreateLogicalAnd(
                                getValue(operExpr.args[0]),
                                getValue(operExpr.args[1]),
                                resName);
                        break;
                    case IR_LOR:
                        res = builder->CreateLogicalOr(
                                getValue(operExpr.args[0]),
                                getValue(operExpr.args[1]),
                                resName);
                        break;
                    case IR_EQ:
                        res = builder->CreateICmpEQ(
                                getValue(operExpr.args[0]),
                                getValue(operExpr.args[1]),
                                resName);
                        break;
                    case IR_NE:
                        res = builder->CreateICmpNE(
                                getValue(operExpr.args[0]),
                                getValue(operExpr.args[1]),
                                resName);
                        break;
                    case IR_GT:
                        if (dirType->isSigned()) {
                            res = builder->CreateICmpSGT(
                                    getValue(operExpr.args[0]),
                                    getValue(operExpr.args[1]),
                                    resName);
                        }
                        else {
                            res = builder->CreateICmpUGT(
                                    getValue(operExpr.args[0]),
                                    getValue(operExpr.args[1]),
                                    resName);
                        }
                        break;
                    case IR_LT:
                        if (dirType->isSigned()) {
                            res = builder->CreateICmpSLT(
                                    getValue(operExpr.args[0]),
                                    getValue(operExpr.args[1]),
                                    resName);
                        }
                        else {
                            res = builder->CreateICmpULT(
                                    getValue(operExpr.args[0]),
                                    getValue(operExpr.args[1]),
                                    resName);
                        }
                        break;
                    case IR_GE:
                        if (dirType->isSigned()) {
                            res = builder->CreateICmpSGE(
                                    getValue(operExpr.args[0]),
                                    getValue(operExpr.args[1]),
                                    resName);
                        }
                        else {
                            res = builder->CreateICmpUGE(
                                    getValue(operExpr.args[0]),
                                    getValue(operExpr.args[1]),
                                    resName);
                        }
                        break;
                    case IR_LE:
                        if (dirType->isSigned()) {
                            res = builder->CreateICmpSLE(
                                    getValue(operExpr.args[0]),
                                    getValue(operExpr.args[1]),
                                    resName);
                        }
                        else {
                            res = builder->CreateICmpULE(
                                    getValue(operExpr.args[0]),
                                    getValue(operExpr.args[1]),
                                    resName);
                        }
                        break;
                    case IR_LOAD: {
                        auto const &ptrType = dynamic_cast<IR_TypePtr const &>(
                                *operExpr.args[0].getType());
                        res = builder->CreateLoad(
                                getTypeFromIR(*ptrType.child),
                                getValue(operExpr.args[0]),
                                resName);
                        break;
                    }
                    case IR_STORE:
                        builder->CreateStore(
                                getValue(operExpr.args[1]),
                                getValue(operExpr.args[0]));
                        break;
                    case IR_EXTRACT:
                        res = builder->CreateExtractValue(
                                getValue(operExpr.args[0]),
                                { operExpr.args[1].castValTo<uint32_t>() },
                                resName);
                        break;
                    case IR_INSERT:
                        res = builder->CreateInsertValue(
                                getValue(operExpr.args[0]),
                                getValue(operExpr.args[2]),
                                { operExpr.args[1].castValTo<uint32_t>() },
                                resName);
                        break;
                    case IR_MOV:
                        NOT_IMPLEMENTED("mov");
                    default:
                        semanticError("Wrong op value");
                }
                if (res)
                    regsMap.emplace(*node.res, res);
                break;
            }

            case IR_Expr::ALLOCATION: {
                auto allocExpr = node.body->getAlloc();
                Value *res = builder->CreateAlloca(getTypeFromIR(*allocExpr.type),
                                      nullptr,
                                      node.res->to_reg_name());
                regsMap.emplace(*node.res, res);
                break;
            }

            case IR_Expr::CAST: {
                auto const &castExpr = node.body->getCast();
                std::string resName = node.res.has_value() ? node.res->to_reg_name() : "";
                Value *res = nullptr;
                switch (castExpr.castOp) {
                    case IR_ExprCast::BITCAST:
                        res = builder->CreateBitCast(
                                getValue(castExpr.arg),
                                getTypeFromIR(*castExpr.dest),
                                resName);
                        break;
                    case IR_ExprCast::SEXT:
                        res = builder->CreateSExt(
                                getValue(castExpr.arg),
                                getTypeFromIR(*castExpr.dest),
                                resName);
                        break;
                    case IR_ExprCast::ZEXT:
                        res = builder->CreateZExt(
                                getValue(castExpr.arg),
                                getTypeFromIR(*castExpr.dest),
                                resName);
                        break;
                    case IR_ExprCast::TRUNC:
                        res = builder->CreateTrunc(
                                getValue(castExpr.arg),
                                getTypeFromIR(*castExpr.dest),
                                resName);
                        break;
                    case IR_ExprCast::FPTOUI:
                        res = builder->CreateFPToUI(
                                getValue(castExpr.arg),
                                getTypeFromIR(*castExpr.dest),
                                resName);
                        break;
                    case IR_ExprCast::FPTOSI:
                        res = builder->CreateFPToSI(
                                getValue(castExpr.arg),
                                getTypeFromIR(*castExpr.dest),
                                resName);
                        break;
                    case IR_ExprCast::UITOFP:
                        res = builder->CreateUIToFP(
                                getValue(castExpr.arg),
                                getTypeFromIR(*castExpr.dest),
                                resName);
                        break;
                    case IR_ExprCast::SITOFP:
                        res = builder->CreateSIToFP(
                                getValue(castExpr.arg),
                                getTypeFromIR(*castExpr.dest),
                                resName);
                        break;
                    case IR_ExprCast::PTRTOI:
                        res = builder->CreatePtrToInt(
                                getValue(castExpr.arg),
                                getTypeFromIR(*castExpr.dest),
                                resName);
                        break;
                    case IR_ExprCast::ITOPTR:
                        res = builder->CreateIntToPtr(
                                getValue(castExpr.arg),
                                getTypeFromIR(*castExpr.dest),
                                resName);
                        break;
                    default:
                        semanticError("Wrong cast type");
                }
                if (res)
                    regsMap.emplace(*node.res, res);
                break;
            }

            case IR_Expr::CALL: {
                auto const &callExpr = node.body->getCall();
                auto func = functions.at(callExpr.funcId);
                std::vector<Value*> args;
                for (auto const &arg : callExpr.args)
                    args.push_back(getValue(arg));
                if (node.res.has_value()) {
                    auto res = builder->CreateCall(func, args, node.res->to_reg_name());
                    regsMap.emplace(*node.res, res);
                }
                else {
                    builder->CreateCall(func, args);
                }
                break;
            }

            case IR_Expr::TERM: {
                semanticError("LLVM Term node in general list");
            }

            case IR_Expr::PHI: {
                semanticError("LLVM Phi node in general list");
            }

            default: {
                semanticError("Wrong node type");
            }
        }
    }

    if (!cfgBlock.termNode.has_value())
        semanticError("Unterminated block");
    IR_ExprTerminator const *termNode = cfgBlock.getTerminator();
    // Branches and jumps are handled when blocks linked
    if (termNode->termType == IR_ExprTerminator::RET) {
        if (termNode->arg.has_value())
            builder->CreateRet(getValue(*termNode->arg));
        else
            builder->CreateRet(nullptr);
    }
}
