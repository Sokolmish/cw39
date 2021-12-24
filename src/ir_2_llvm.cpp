#include "ir_2_llvm.hpp"
#include "utils.hpp"
#include <fmt/core.h>

#include "llvm/ADT/APInt.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"
//#include "llvm/ExecutionEngine/ExecutionEngine.h"
//#include "llvm/Support/TargetSelect.h"

using namespace llvm;

IR2LLVM::IR2LLVM(std::shared_ptr<ControlFlowGraph> cfg) : cfg(cfg) {
    context = new LLVMContext();
    module = new Module("top", *context);
    builder = new IRBuilder<>(*context);

    Function *dummyFunc = Function::Create(FunctionType::get(builder->getVoidTy(), false),
                                           Function::InternalLinkage, "__dummy", module);
    BasicBlock *dummyEntryBlock = BasicBlock::Create(*context, "dummy", dummyFunc);
    builder->SetInsertPoint(dummyEntryBlock);

    createGlobals();
    createPrototypes();

    raw_string_ostream output(res);
    module->print(output, nullptr);

    delete builder;
    delete module;
    delete context;
}

std::string IR2LLVM::getRes() const {
    return res;
}

void IR2LLVM::createTypes() {
    NOT_IMPLEMENTED("Aggregate types");
}

void IR2LLVM::createGlobals() {
    for (auto const &[sId, str] : cfg->strings) {
        builder->CreateGlobalString(StringRef(str), fmt::format(".str{}", sId));
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
    else {
        NOT_IMPLEMENTED("LLVM indirect type");
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

void IR2LLVM::createPrototypes() {
    for (auto const &[fId, func] : cfg->prototypes) {
        auto irFuncType = std::dynamic_pointer_cast<IR_TypeFunc>(func.fullType);
        llvm::Type *retType = getTypeFromIR(*irFuncType->ret);
        std::vector<llvm::Type*> args;
        for (auto const &arg : irFuncType->args)
            args.push_back(getTypeFromIR(*arg));
        auto ftype = FunctionType::get(retType, args, irFuncType->isVariadic);
        Function::Create(ftype, Function::ExternalLinkage, func.getName(), *module);
    }
}
