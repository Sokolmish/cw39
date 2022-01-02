#include "ir_2_llvm.hpp"

#include <map>
#include <set>
#include <deque>
#include <fmt/core.h>

#include "utils.hpp"
#include "transformations/dominators.hpp"

#include "llvm/IR/IRBuilder.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/LLVMContext.h"

#include "llvm/ADT/APInt.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"

//#include "llvm/IR/Verifier.h"
//#include "llvm/ExecutionEngine/ExecutionEngine.h"
//#include "llvm/Support/TargetSelect.h"

using namespace llvm;

class IR2LLVM::IR2LLVM_Impl {
public:
    IR2LLVM *parent;

    std::unique_ptr<Dominators> doms;

    std::unique_ptr<LLVMContext> context;
    std::unique_ptr<Module> module;
    std::unique_ptr<IRBuilder<>> builder;

    void createTypes();
    void createGlobals();
    void createPrototypes();

    void createFunctions();
    void createBlock(int id);
    void buildOperation(IR_Node const &node);
    void buildCast(IR_Node const &node);

    Type* getTypeFromIR(IR_Type const &ir_type);
    Constant* getConstantFromIR(IRval const &val) const;
    Value* getValue(IRval const &val);

    Function *curFunction = nullptr;

    std::map<IRval, Value*, IRval::ComparatorVersions> regsMap;
    std::map<int, BasicBlock*> blocksMap;
    std::map<int, Value*> strings;
    std::map<int, GlobalVariable*> globals;
    std::map<int, Function*> functions;
    std::map<int, Type*> structTypes;

    std::map<IRval, PHINode*, IRval::ComparatorVersions> unfilledPhis;

    explicit IR2LLVM_Impl(IR2LLVM *par);

    IR2LLVM_Impl(IR2LLVM_Impl const&) = delete;
    IR2LLVM_Impl& operator=(IR2LLVM_Impl const&) = delete;
};

using IR2LLVM_Impl = IR2LLVM::IR2LLVM_Impl;

IR2LLVM::IR2LLVM(std::shared_ptr<ControlFlowGraph> cfg) : cfg(std::move(cfg)) {
    impl = std::make_unique<IR2LLVM_Impl>(this);
}

std::string IR2LLVM::getRes() const {
    return llvmIR;
}

IR2LLVM::~IR2LLVM() = default;


IR2LLVM_Impl::IR2LLVM_Impl(IR2LLVM *par) : parent(par) {
    doms = std::make_unique<Dominators>(parent->cfg);

    context = std::make_unique<LLVMContext>();
    module = std::make_unique<Module>("top", *context);
    builder = std::make_unique<IRBuilder<>>(*context);

    Function *dummyFunc = Function::Create(FunctionType::get(builder->getVoidTy(), false),
                                           Function::InternalLinkage, "__dummy_func", *module);
    BasicBlock *dummyEntryBlock = BasicBlock::Create(*context, "dummy_block", dummyFunc);
    builder->SetInsertPoint(dummyEntryBlock);
    builder->CreateRet(nullptr);

    createTypes();
    createGlobals();
    createPrototypes();
    createFunctions();

    raw_string_ostream output(parent->llvmIR);
    module->print(output, nullptr);

    builder.reset();
    module.reset();
    context.reset();
}


void IR2LLVM_Impl::createTypes() {
    for (auto const &[sId, tstruct] : parent->cfg->getStructs()) {
        std::vector<Type*> elements;
        for (auto const &elem : tstruct->fields)
            elements.push_back(getTypeFromIR(*elem.irType));
        auto type =
                StructType::create(*context, elements, fmt::format("struct{}", tstruct->structId));
        structTypes.emplace(tstruct->structId, type);
    }
}

void IR2LLVM_Impl::createGlobals() {
    for (auto const &[sId, str] : parent->cfg->getStrings()) {
        auto res = builder->CreateGlobalStringPtr(StringRef(str), fmt::format(".str{}", sId));
        strings.emplace(sId, res);
    }
    for (auto const &[gId, global] : parent->cfg->getGlobals()) {
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

llvm::Type* IR2LLVM_Impl::getTypeFromIR(const IR_Type &ir_type) {
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
            case IR_TypeDirect::F64:
                return builder->getDoubleTy();
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
        auto const &funType = dynamic_cast<IR_TypeFunc const &>(ir_type);
        std::vector<Type*> args;
        for (auto const &arg : funType.args)
            args.push_back(getTypeFromIR(*arg));
        return FunctionType::get(getTypeFromIR(*funType.ret), args, funType.isVariadic);
    }
    else {
        semanticError("Unknown type");
    }
}

llvm::Constant *IR2LLVM_Impl::getConstantFromIR(IRval const &val) const {
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
            return ConstantFP::get(builder->getFloatTy(), val.castValTo<float>());
        case IR_TypeDirect::F64:
            return ConstantFP::get(builder->getDoubleTy(), val.castValTo<double>());
    }
    throw;
}

llvm::Value *IR2LLVM_Impl::getValue(const IRval &val) {
    switch (val.valClass) {
        case IRval::VAL: {
            auto const &dirType = dynamic_cast<IR_TypeDirect const &>(*val.getType());
            if (dirType.isInteger()) {
                return builder->getIntN(dirType.getBytesSize() * 8, val.castValTo<uint64_t>());
            }
            else if (dirType.isFloat()) {
                if (dirType.getBytesSize() == 4)
                    return ConstantFP::get(builder->getFloatTy(), val.castValTo<float>());
                else
                    return ConstantFP::get(builder->getDoubleTy(), val.castValTo<double>());
            }
            else {
                semanticError("Wrong constant type");
            }
        }

        case IRval::FUN_PTR:
            return functions.at(val.castValTo<int>());

        case IRval::VREG:
            return regsMap.at(val);

        case IRval::GLOBAL:
            return globals.at(val.castValTo<int>());

        case IRval::STRING:
            return strings.at(val.castValTo<int>());

        case IRval::FUN_PARAM:
            return curFunction->getArg(val.castValTo<int>());

        case IRval::UNDEF:
            return UndefValue::get(getTypeFromIR(*val.getType()));
    }
    throw;
}


void IR2LLVM_Impl::createPrototypes() {
    for (auto const &[fId, func] : parent->cfg->getPrototypes()) {
        auto irFuncType = std::dynamic_pointer_cast<IR_TypeFunc>(func.fullType);
        Type *retType = getTypeFromIR(*irFuncType->ret);
        std::vector<Type*> args;
        for (auto const &arg : irFuncType->args)
            args.push_back(getTypeFromIR(*arg));
        auto ftype = FunctionType::get(retType, args, irFuncType->isVariadic);
        auto prototype = Function::Create(ftype, Function::ExternalLinkage, func.getName(), *module);
        functions.emplace(fId, prototype);
    }
}

void IR2LLVM_Impl::createFunctions() {
    for (auto const &[fId, func]: parent->cfg->getFuncs()) {
        auto irFuncType = std::dynamic_pointer_cast<IR_TypeFunc>(func.fullType);
        Type *retType = getTypeFromIR(*irFuncType->ret);
        std::vector<Type *> args;
        for (auto const &arg: irFuncType->args)
            args.push_back(getTypeFromIR(*arg));
        auto ftype = FunctionType::get(retType, args, irFuncType->isVariadic);

        curFunction = Function::Create(ftype, Function::ExternalLinkage, func.getName(), *module);
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
            auto const &cfgBlock = parent->cfg->block(blockId);
            IR_ExprTerminator const *term = cfgBlock.getTerminator();
            if (term->termType == IR_ExprTerminator::JUMP) {
                builder->SetInsertPoint(blocksMap.at(blockId));
                builder->CreateBr(blocksMap.at(cfgBlock.next.at(0)));
            }
            else if (term->termType == IR_ExprTerminator::BRANCH) {
                builder->SetInsertPoint(blocksMap.at(blockId));

                // TODO: check types
                Value *cond = getValue(*term->arg);
                if (cond->getType()->getIntegerBitWidth() != 1) {
                    cond = builder->CreateICmpNE(
                            cond,
                            builder->getIntN(term->arg->getType()->getBytesSize() * 8, 0ULL));
                }

                builder->CreateCondBr(
                        cond,
                        blocksMap.at(cfgBlock.next.at(0)),
                        blocksMap.at(cfgBlock.next.at(1)));
            }
        }

        // Link phis
        for (int blockId: visited) {
            auto const &cfgBlock = parent->cfg->block(blockId);
            for (auto const &phiNode: cfgBlock.phis) {
                auto cfgPhiFunc = phiNode.body->getPhi();
                auto ph = unfilledPhis.at(*phiNode.res);
                for (auto const &[pos, arg]: cfgPhiFunc.args) {
                    ph->addIncoming(getValue(arg), blocksMap.at(cfgBlock.prev.at(pos)));
                }
            }
        }
        unfilledPhis.clear();
    }
}

void IR2LLVM_Impl::createBlock(int id) {
    auto const &cfgBlock = parent->cfg->block(id);
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
                buildOperation(node);
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
                buildCast(node);
                break;
            }

            case IR_Expr::CALL: {
                auto const &callExpr = node.body->getCall();

                std::vector<Value *> args;
                for (auto const &arg: callExpr.args)
                    args.push_back(getValue(arg));

                if (callExpr.isIndirect()) {
                    auto ptrVal = callExpr.getFuncPtr();
                    auto irPtrType = std::dynamic_pointer_cast<IR_TypePtr>(ptrVal.getType());
                    auto irFuncType = irPtrType->child;
                    auto funcTy = static_cast<FunctionType *>(getTypeFromIR(*irFuncType)); // TODO: static cast...
                    if (node.res.has_value()) {
                        auto res = builder->CreateCall(
                                funcTy, getValue(ptrVal), args, node.res->to_reg_name());
                        regsMap.emplace(*node.res, res);
                    }
                    else {
                        builder->CreateCall(funcTy, getValue(ptrVal), args);
                    }
                }
                else {
                    auto func = functions.at(callExpr.getFuncId());
                    if (node.res.has_value()) {
                        auto res = builder->CreateCall(func, args, node.res->to_reg_name());
                        regsMap.emplace(*node.res, res);
                    }
                    else {
                        builder->CreateCall(func, args);
                    }
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

void IR2LLVM_Impl::buildOperation(IR_Node const &node) {
    const IR_ExprOper &oper = node.body->getOper();

    auto dirType = std::dynamic_pointer_cast<IR_TypeDirect>(oper.args[0].getType());
    std::string name = node.res.has_value() ? node.res->to_reg_name() : "";
    Value *res = nullptr;

    switch (oper.op) {
        case IR_ExprOper::MUL:
            if (dirType->isInteger())
                res = builder->CreateMul(getValue(oper.args[0]), getValue(oper.args[1]), name);
            else if (dirType->isFloat())
                res = builder->CreateFMul(getValue(oper.args[0]), getValue(oper.args[1]), name);
            else
                semanticError("Wrong mul types");
            break;

        case IR_ExprOper::DIV:
            if (dirType->isInteger()) {
                if (dirType->isSigned())
                    res = builder->CreateSDiv(getValue(oper.args[0]), getValue(oper.args[1]), name);
                else
                    res = builder->CreateUDiv(getValue(oper.args[0]), getValue(oper.args[1]), name);
            }
            else if (dirType->isFloat())
                res = builder->CreateFDiv(getValue(oper.args[0]), getValue(oper.args[1]), name);
            else
                semanticError("Wrong div types");
            break;

        case IR_ExprOper::REM:
            if (dirType->isInteger()) {
                if (dirType->isSigned())
                    res = builder->CreateSRem(getValue(oper.args[0]), getValue(oper.args[1]), name);
                else
                    res = builder->CreateURem(getValue(oper.args[0]), getValue(oper.args[1]), name);
            }
            else if (dirType->isFloat())
                res = builder->CreateFRem(getValue(oper.args[0]), getValue(oper.args[1]), name);
            else
                semanticError("Wrong div types");
            break;

        case IR_ExprOper::ADD:
            if (dirType->isInteger())
                res = builder->CreateAdd(getValue(oper.args[0]), getValue(oper.args[1]), name);
            else if (dirType->isFloat())
                res = builder->CreateFAdd(getValue(oper.args[0]), getValue(oper.args[1]), name);
            else
                semanticError("Wrong add types");
            break;

        case IR_ExprOper::SUB:
            if (dirType->isInteger())
                res = builder->CreateSub(getValue(oper.args[0]), getValue(oper.args[1]), name);
            else if (dirType->isFloat())
                res = builder->CreateFSub(getValue(oper.args[0]), getValue(oper.args[1]), name);
            else
                semanticError("Wrong sub types");
            break;

        case IR_ExprOper::SHR:
            if (dirType->isSigned())
                res = builder->CreateAShr(getValue(oper.args[0]), getValue(oper.args[1]), name);
            else
                res = builder->CreateLShr(getValue(oper.args[0]), getValue(oper.args[1]), name);
            break;

        case IR_ExprOper::SHL:
            res = builder->CreateShl(getValue(oper.args[0]), getValue(oper.args[1]), name);
            break;

        case IR_ExprOper::XOR:
            res = builder->CreateXor(getValue(oper.args[0]), getValue(oper.args[1]), name);
            break;

        case IR_ExprOper::AND:
            res = builder->CreateAnd(getValue(oper.args[0]), getValue(oper.args[1]), name);
            break;

        case IR_ExprOper::OR:
            res = builder->CreateOr(getValue(oper.args[0]), getValue(oper.args[1]), name);
            break;

        case IR_ExprOper::LAND:
            res = builder->CreateLogicalAnd(getValue(oper.args[0]), getValue(oper.args[1]), name);
            break;

        case IR_ExprOper::LOR:
            res = builder->CreateLogicalOr(getValue(oper.args[0]), getValue(oper.args[1]), name);
            break;

        case IR_ExprOper::EQ:
            res = builder->CreateICmpEQ(getValue(oper.args[0]), getValue(oper.args[1]), name);
            break;

        case IR_ExprOper::NE:
            res = builder->CreateICmpNE(getValue(oper.args[0]), getValue(oper.args[1]), name);
            break;

        case IR_ExprOper::GT:
            if (dirType->isSigned())
                res = builder->CreateICmpSGT(getValue(oper.args[0]), getValue(oper.args[1]), name);
            else
                res = builder->CreateICmpUGT(getValue(oper.args[0]), getValue(oper.args[1]), name);
            break;

        case IR_ExprOper::LT:
            if (dirType->isSigned())
                res = builder->CreateICmpSLT(getValue(oper.args[0]), getValue(oper.args[1]), name);
            else
                res = builder->CreateICmpULT(getValue(oper.args[0]), getValue(oper.args[1]), name);
            break;

        case IR_ExprOper::GE:
            if (dirType->isSigned())
                res = builder->CreateICmpSGE(getValue(oper.args[0]), getValue(oper.args[1]), name);
            else
                res = builder->CreateICmpUGE(getValue(oper.args[0]), getValue(oper.args[1]), name);
            break;

        case IR_ExprOper::LE:
            if (dirType->isSigned())
                res = builder->CreateICmpSLE(getValue(oper.args[0]), getValue(oper.args[1]), name);
            else
                res = builder->CreateICmpULE(getValue(oper.args[0]), getValue(oper.args[1]), name);
            break;

        case IR_ExprOper::LOAD: {
            auto const &ptrTp = dynamic_cast<IR_TypePtr const &>(*oper.args[0].getType());
            res = builder->CreateLoad(getTypeFromIR(*ptrTp.child), getValue(oper.args[0]), name);
            break;
        }

        case IR_ExprOper::STORE:
            builder->CreateStore(getValue(oper.args[1]), getValue(oper.args[0]));
            break;

        case IR_ExprOper::EXTRACT:
            res = builder->CreateExtractValue(
                    getValue(oper.args[0]),
                    { oper.args[1].castValTo<uint32_t>() },
                    name);
            break;

        case IR_ExprOper::INSERT:
            res = builder->CreateInsertValue(
                    getValue(oper.args[0]), getValue(oper.args[2]),
                    { oper.args[1].castValTo<uint32_t>() },
                    name);
            break;

        case IR_ExprOper::MOV:
            NOT_IMPLEMENTED("mov");

        default:
            semanticError("Wrong op value");
    }
    if (res) {
        regsMap.emplace(*node.res, res);
    }
}

void IR2LLVM_Impl::buildCast(IR_Node const &node) {
    auto const &castExpr = node.body->getCast();

    auto argVal = getValue(castExpr.arg);
    auto destType = getTypeFromIR(*castExpr.dest);
    std::string name = node.res.has_value() ? node.res->to_reg_name() : "";
    Value *res = nullptr;

    switch (castExpr.castOp) {
        case IR_ExprCast::BITCAST:
            res = builder->CreateBitCast(argVal, destType, name);
            break;

        case IR_ExprCast::SEXT:
            res = builder->CreateSExt(argVal, destType, name);
            break;

        case IR_ExprCast::ZEXT:
            res = builder->CreateZExt(argVal, destType, name);
            break;

        case IR_ExprCast::TRUNC:
            res = builder->CreateTrunc(argVal, destType, name);
            break;

        case IR_ExprCast::FPTOUI:
            res = builder->CreateFPToUI(argVal, destType, name);
            break;

        case IR_ExprCast::FPTOSI:
            res = builder->CreateFPToSI(argVal, destType, name);
            break;

        case IR_ExprCast::UITOFP:
            res = builder->CreateUIToFP(argVal, destType, name);
            break;

        case IR_ExprCast::SITOFP:
            res = builder->CreateSIToFP(argVal, destType, name);
            break;

        case IR_ExprCast::PTRTOI:
            res = builder->CreatePtrToInt(argVal, destType, name);
            break;

        case IR_ExprCast::ITOPTR:
            res = builder->CreateIntToPtr(argVal, destType, name);
            break;

        case IR_ExprCast::FPTRUNC:
            res = builder->CreateFPTrunc(argVal, destType, name);
            break;

        case IR_ExprCast::FPEXT:
            res = builder->CreateFPExt(argVal, destType, name);
            break;

        default:
            semanticError("Wrong cast type");
    }
    if (res) {
        regsMap.emplace(*node.res, res);
    }
}
