#include "ir_2_llvm.hpp"

#include <map>
#include <set>
#include <deque>
#include <fmt/core.h>

#include "utils.hpp"
#include "transformations/graph_info.hpp"

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

#include "llvm/IR/Verifier.h"

//#include "llvm/ExecutionEngine/ExecutionEngine.h"
//#include "llvm/Support/TargetSelect.h"

using namespace llvm;

class IR2LLVM::IR2LLVM_Impl {
public:
    IR2LLVM *parent;

    std::unique_ptr<GraphInfo> gInfo;

    std::unique_ptr<LLVMContext> context;
    std::unique_ptr<Module> module;
    std::unique_ptr<IRBuilder<>> builder;

    void createTypes();
    void createGlobals();
    void createPrototypes();

    void createFunctions();
    void createBlock(int id);

    void buildOperation(IR_Node const &node);
    void buildMemOp(IR_Node const &node);
    void buildAccessOp(IR_Node const &node);
    void buildCast(IR_Node const &node);
    void buildCall(IR_Node const &node);

    Type* getType(IR_Type const &ir_type);
    Constant* getConstant(IRval const &val);
    Value* getValue(IRval const &val);

    Function *curFunction = nullptr;

    std::map<IRval, Value*, IRval::Comparator> regsMap;
    std::map<int, BasicBlock*> blocksMap;
    std::map<int, Value*> strings;
    std::map<int, GlobalVariable*> globals;
    std::map<int, Function*> functions;
    std::map<int, Type*> structTypes;

    std::map<IRval, PHINode*, IRval::Comparator> unfilledPhis;

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
    gInfo = std::make_unique<GraphInfo>(parent->cfg);

    context = std::make_unique<LLVMContext>();
    module = std::make_unique<Module>("top", *context);
    builder = std::make_unique<IRBuilder<>>(*context);

    Function *dummyFunc = Function::Create(FunctionType::get(builder->getVoidTy(), false),
                                           Function::InternalLinkage, "__dummy_func", *module);
    BasicBlock *dummyEntryBlock = BasicBlock::Create(*context, "__dummy_block", dummyFunc);
    builder->SetInsertPoint(dummyEntryBlock);
    builder->CreateRet(nullptr);

    createTypes();
    createGlobals();
    createPrototypes();
    createFunctions();

    dummyFunc->eraseFromParent();

    raw_string_ostream output(parent->llvmIR);
    module->print(output, nullptr);

    builder.reset();
    module.reset();
    context.reset();
    gInfo.reset();
}


void IR2LLVM_Impl::createTypes() {
    for (auto const &[sId, tstruct] : parent->cfg->getStructs()) {
        std::vector<Type*> elements;
        for (auto const &elem : tstruct->fields)
            elements.push_back(getType(*elem.irType));
        std::string structName = fmt::format("struct{}", tstruct->structId);
        auto type = StructType::create(*context, elements, structName, false);
        structTypes.emplace(tstruct->structId, type);
    }
}

void IR2LLVM_Impl::createGlobals() {
    for (auto const &[sId, str] : parent->cfg->getStrings()) {
        Value *res = builder->CreateGlobalStringPtr(StringRef(str), fmt::format(".str{}", sId));
        strings.emplace(sId, res);
    }
    for (auto const &[gId, global] : parent->cfg->getGlobals()) {
        auto ptrType = std::dynamic_pointer_cast<IR_TypePtr>(global.type);
        auto gType = ptrType->child;
        module->getOrInsertGlobal(global.name, getType(*gType));
        GlobalVariable *gVar = module->getNamedGlobal(global.name);
        gVar->setLinkage(GlobalValue::InternalLinkage);
        gVar->setAlignment(Align(8));
        gVar->setInitializer(getConstant(global.init));
        gVar->setConstant(false);

        globals.emplace(gId, gVar);
    }
}

Type* IR2LLVM_Impl::getType(const IR_Type &ir_type) {
    if (ir_type.type == IR_Type::DIRECT) {
        auto const &dirType = dynamic_cast<IR_TypeDirect const &>(ir_type);
        switch (dirType.spec) {
            case IR_TypeDirect::VOID:
                return builder->getVoidTy();
            case IR_TypeDirect::BOOL:
                return builder->getInt1Ty();
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

        // Void pointers are not allowed in LLVM, so they replaced with i8*
        if (ptrType.child->type == IR_Type::DIRECT) {
            auto childType = std::dynamic_pointer_cast<IR_TypeDirect>(ptrType.child);
            if (childType->spec == IR_TypeDirect::VOID)
                return PointerType::getUnqual(builder->getInt8Ty());
        }

        return PointerType::getUnqual(getType(*ptrType.child));
    }
    else if (ir_type.type == IR_Type::ARRAY) {
        auto const &arrType = dynamic_cast<IR_TypeArray const &>(ir_type);
        return ArrayType::get(getType(*arrType.child), arrType.size);
    }
    else if (ir_type.type == IR_Type::TSTRUCT) {
        auto const &structType = dynamic_cast<IR_TypeStruct const &>(ir_type);
        return structTypes.at(structType.structId);
    }
    else if (ir_type.type == IR_Type::FUNCTION) {
        auto const &funType = dynamic_cast<IR_TypeFunc const &>(ir_type);
        std::vector<Type*> args;
        for (auto const &arg : funType.args)
            args.push_back(getType(*arg));
        return FunctionType::get(getType(*funType.ret), args, funType.isVariadic);
    }
    else {
        semanticError("Unknown type");
    }
}

Constant *IR2LLVM_Impl::getConstant(IRval const &val) {
    if (!val.isConstant())
        semanticError("LLVM Not a constant value");
    return dyn_cast<Constant>(getValue(val));
}

Value* IR2LLVM_Impl::getValue(const IRval &val) {
    switch (val.getValueClass()) {
        case IRval::VAL: {
            auto const &dirType = dynamic_cast<IR_TypeDirect const &>(*val.getType());
            if (dirType.isInteger()) {
                return builder->getIntN(dirType.getBytesSize() * 8, val.castValTo<uint64_t>());
            }
            else if (dirType.isFloat()) {
                if (dirType.getBytesSize() == 4)
                    return ConstantFP::get(builder->getFloatTy(), APFloat(val.castValTo<float>()));
                else
                    return ConstantFP::get(builder->getDoubleTy(), APFloat(val.castValTo<double>()));
            }
            else {
                semanticError("Wrong constant type");
            }
        }

        case IRval::AGGREGATE: {
            Type *aggregateType = getType(*val.getType());
            std::vector<Constant *> args;
            for (auto const &elem : val.getAggregateVal())
                args.push_back(dyn_cast<Constant>(getValue(elem)));

            if (val.getType()->type == IR_Type::TSTRUCT)
                return ConstantStruct::get(dyn_cast<StructType>(aggregateType), args);
            else if (val.getType()->type == IR_Type::ARRAY)
                return ConstantArray::get(dyn_cast<ArrayType>(aggregateType), args);
            else
                semanticError("Wrong aggregate initializer type");
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
            return UndefValue::get(getType(*val.getType()));

        case IRval::ZEROINIT:
            return ConstantAggregateZero::get(getType(*val.getType()));
    }
    throw;
}


void IR2LLVM_Impl::createPrototypes() {
    for (auto const &[fId, func] : parent->cfg->getPrototypes()) {
        auto irFuncType = std::dynamic_pointer_cast<IR_TypeFunc>(func.fullType);
        Type *retType = getType(*irFuncType->ret);
        std::vector<Type*> args;
        for (auto const &arg : irFuncType->args)
            args.push_back(getType(*arg));
        auto ftype = FunctionType::get(retType, args, irFuncType->isVariadic);
        auto prototype = Function::Create(ftype, Function::ExternalLinkage, func.getName(), *module);
        functions.emplace(fId, prototype);
    }
}

void IR2LLVM_Impl::createFunctions() {
    for (auto const &[fId, func] : parent->cfg->getFuncs()) {
        auto irFuncType = std::dynamic_pointer_cast<IR_TypeFunc>(func.fullType);
        Type *retType = getType(*irFuncType->ret);
        std::vector<Type *> args;
        for (auto const &arg : irFuncType->args)
            args.push_back(getType(*arg));
        auto ftype = FunctionType::get(retType, args, irFuncType->isVariadic);

        curFunction = Function::Create(ftype, Function::ExternalLinkage, func.getName(), *module);
        functions.emplace(fId, curFunction);

        int argsCntr = 0;
        for (auto &arg : curFunction->args()) {
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
            for (int next : gInfo->getChildren(cur))
                queue.push_back(next);

            createBlock(cur);
        }

        // Link blocks
        for (int blockId : visited) {
            auto const &cfgBlock = parent->cfg->block(blockId);
            IR_ExprTerminator const *term = cfgBlock.getTerminator();
            if (term->termType == IR_ExprTerminator::JUMP) {
                builder->SetInsertPoint(blocksMap.at(blockId));
                builder->CreateBr(blocksMap.at(cfgBlock.next.at(0)));
            }
            else if (term->termType == IR_ExprTerminator::BRANCH) {
                builder->SetInsertPoint(blocksMap.at(blockId));
                Value *cond = getValue(*term->arg);
                if (cond->getType()->getIntegerBitWidth() != 1) {
                    Constant *zero = ConstantInt::get(getType(*term->arg->getType()), 0ULL);
                    cond = builder->CreateICmpNE(cond, zero);
                }
                builder->CreateCondBr(cond, blocksMap.at(cfgBlock.next.at(0)),
                                      blocksMap.at(cfgBlock.next.at(1)));
            }
        }

        // Link phis
        for (int blockId : visited) {
            auto const &cfgBlock = parent->cfg->block(blockId);
            for (auto const &phiNode : cfgBlock.phis) {
                auto cfgPhiFunc = phiNode.body->getPhi();
                PHINode *ph = unfilledPhis.at(*phiNode.res);
                for (auto const &[pos, arg] : cfgPhiFunc.args) {
                    ph->addIncoming(getValue(arg), blocksMap.at(cfgBlock.prev.at(pos)));
                }
            }
        }
        unfilledPhis.clear();

        if (verifyFunction(*curFunction, &errs())) {
            errs() << fmt::format("\nVerification failed: {}\n\n\n", func.getName());
        }
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
                getType(*phiNode.res->getType()),
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
            case IR_Expr::MEMORY: {
                buildMemOp(node);
                break;
            }
            case IR_Expr::ACCESS: {
                buildAccessOp(node);
                break;
            }
            case IR_Expr::ALLOCATION: {
                auto allocExpr = node.body->getAlloc();
                std::string resName = node.res->to_reg_name();
                Value *res = builder->CreateAlloca(getType(*allocExpr.type), nullptr, resName);
                regsMap.emplace(*node.res, res);
                break;
            }
            case IR_Expr::CAST: {
                buildCast(node);
                break;
            }
            case IR_Expr::CALL: {
                buildCall(node);
                break;
            }
            case IR_Expr::TERM:
                semanticError("LLVM Term node in general list");
            case IR_Expr::PHI:
                semanticError("LLVM phi node in general list");
            default:
                semanticError("Wrong node type");
        }
    }

    if (!cfgBlock.termNode.has_value())
        semanticError("Unterminated block");
    IR_ExprTerminator const *termNode = cfgBlock.getTerminator();
    // Branches and jumps are handled when blocks linking
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

        case IR_ExprOper::MOV:
            res = getValue(oper.args[0]);
            break;

        default:
            semanticError("Wrong op value");
    }
    regsMap.emplace(*node.res, res);
}

void IR2LLVM_Impl::buildMemOp(IR_Node const &node) {
    const IR_ExprMem &oper = node.body->getMem();
    switch (oper.op) {
        case IR_ExprMem::LOAD: {
            auto const &ptrTp = dynamic_cast<IR_TypePtr const &>(*oper.addr.getType());
            std::string name = node.res.has_value() ? node.res->to_reg_name() : "";
            Value *res = builder->CreateLoad(getType(*ptrTp.child), getValue(oper.addr), name);
            regsMap.emplace(*node.res, res);
            break;
        }
        case IR_ExprMem::STORE: {
            builder->CreateStore(getValue(*oper.val), getValue(oper.addr));
            break;
        }
        default:
            semanticError("Wrong op value");
    }
}

void IR2LLVM::IR2LLVM_Impl::buildAccessOp(const IR_Node &node) {
    const IR_ExprAccess &acc = node.body->getAccess();
    std::string name = node.res.has_value() ? node.res->to_reg_name() : "";

    if (acc.op == IR_ExprAccess::GEP) {
        std::vector<Value *> indices;
        indices.reserve(acc.indices.size());
        for (auto const &ind : acc.indices)
            indices.push_back(getValue(ind));

        Value *res = builder->CreateInBoundsGEP( // TODO: inbounds?
                getType(*dynamic_cast<IR_TypePtr const &>(*acc.base.getType()).child),
                getValue(acc.base), indices, name);
        regsMap.emplace(*node.res, res);
    }
    else if (isInList(acc.op, { IR_ExprAccess::EXTRACT, IR_ExprAccess::INSERT })) {
        std::vector<uint32_t> indices;
        indices.reserve(acc.indices.size());
        for (auto const &ind : acc.indices) {
            if (ind.getValueClass() != IRval::VAL)
                semanticError("Non constant argument of EXTRACT/INSERT");
            indices.push_back(ind.castValTo<uint32_t>());
        }

        Value *res;
        if (acc.op == IR_ExprAccess::EXTRACT)
            res = builder->CreateExtractValue(getValue(acc.base), indices, name);
        else
            res = builder->CreateInsertValue(getValue(acc.base), getValue(*acc.val), indices, name);
        regsMap.emplace(*node.res, res);
    }
    else {
        semanticError("Wrong access op type");
    }
}

static auto getCastOp(IR_ExprCast::CastType op) {
    switch (op) {
        case IR_ExprCast::BITCAST:
            return &IRBuilder<>::CreateBitCast;
        case IR_ExprCast::SEXT:
            return &IRBuilder<>::CreateSExt;
        case IR_ExprCast::ZEXT:
            return &IRBuilder<>::CreateZExt;
        case IR_ExprCast::TRUNC:
            return &IRBuilder<>::CreateTrunc;
        case IR_ExprCast::FPTOUI:
            return &IRBuilder<>::CreateFPToUI;
        case IR_ExprCast::FPTOSI:
            return &IRBuilder<>::CreateFPToSI;
        case IR_ExprCast::UITOFP:
            return &IRBuilder<>::CreateUIToFP;
        case IR_ExprCast::SITOFP:
            return &IRBuilder<>::CreateSIToFP;
        case IR_ExprCast::PTRTOI:
            return &IRBuilder<>::CreatePtrToInt;
        case IR_ExprCast::ITOPTR:
            return &IRBuilder<>::CreateIntToPtr;
        case IR_ExprCast::FPTRUNC:
            return &IRBuilder<>::CreateFPTrunc;
        case IR_ExprCast::FPEXT:
            return &IRBuilder<>::CreateFPExt;
    }
    semanticError("Wrong cast type");
}

void IR2LLVM_Impl::buildCast(IR_Node const &node) {
    auto const &castExpr = node.body->getCast();

    Value *argVal = getValue(castExpr.arg);
    Type *destType = getType(*castExpr.dest);
    std::string name = node.res.has_value() ? node.res->to_reg_name() : "";

    Value *res = ((*builder).*getCastOp(castExpr.castOp))(argVal, destType, name);
    if (res)
        regsMap.emplace(*node.res, res);
}

void IR2LLVM::IR2LLVM_Impl::buildCall(const IR_Node &node) {
    auto const &callExpr = node.body->getCall();

    std::vector<Value *> args;
    for (IRval const &arg: callExpr.args)
        args.push_back(getValue(arg));

    if (callExpr.isIndirect()) {
        IRval ptrVal = callExpr.getFuncPtr();
        auto irPtrType = std::dynamic_pointer_cast<IR_TypePtr>(ptrVal.getType());
        auto irFuncType = irPtrType->child;
        auto funcTy = dyn_cast<FunctionType>(getType(*irFuncType));
        if (node.res.has_value()) {
            Value *res = builder->CreateCall(
                    funcTy, getValue(ptrVal), args, node.res->to_reg_name());
            regsMap.emplace(*node.res, res);
        }
        else {
            builder->CreateCall(funcTy, getValue(ptrVal), args);
        }
    }
    else {
        Function *func = functions.at(callExpr.getFuncId());
        if (node.res.has_value()) {
            Value *res = builder->CreateCall(func, args, node.res->to_reg_name());
            regsMap.emplace(*node.res, res);
        }
        else {
            builder->CreateCall(func, args);
        }
    }
}
