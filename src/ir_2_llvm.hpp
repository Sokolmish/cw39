#ifndef __IR_2_LLVM_HPP__
#define __IR_2_LLVM_HPP__

#include <memory>
#include <map>
#include "ir/cfg.hpp"
#include "transformations/dominators.hpp"

#include "llvm/IR/IRBuilder.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/LLVMContext.h"

class IR2LLVM {
private:
    std::shared_ptr<ControlFlowGraph> cfg;
    std::unique_ptr<Dominators> doms;

    llvm::LLVMContext *context;
    llvm::Module *module;
    llvm::IRBuilder<> *builder;

    std::string llvmIR;

    void createTypes();
    void createGlobals();
    void createPrototypes();

    void createFunctions();
    void createBlock(int id);

    llvm::Type* getTypeFromIR(IR_Type const &ir_type);
    llvm::Constant* getConstantFromIR(IRval const &val);
    llvm::Value* getValue(IRval const &val);

    llvm::Function *curFunction;

    std::map<IRval, llvm::Value*, IRval::ComparatorVersions> regsMap;
    std::map<int, llvm::BasicBlock*> blocksMap;
    std::map<int, llvm::Value*> strings;
    std::map<int, llvm::GlobalVariable*> globals;
    std::map<int, llvm::Function*> functions;
    std::map<int, llvm::Type*> structTypes;

    std::map<IRval, llvm::PHINode*, IRval::ComparatorVersions> unfilledPhis;

public:
    IR2LLVM(std::shared_ptr<ControlFlowGraph> cfg);

    IR2LLVM(IR2LLVM const&) = delete;
    IR2LLVM& operator=(IR2LLVM const&) = delete;

    std::string getRes() const;
};


#endif /* __IR_2_LLVM_HPP__ */
