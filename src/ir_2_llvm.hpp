#ifndef __IR_2_LLVM_HPP__
#define __IR_2_LLVM_HPP__

#include <memory>
#include "ir/cfg.hpp"

#include "llvm/IR/IRBuilder.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/LLVMContext.h"

class IR2LLVM {
private:
    std::shared_ptr<ControlFlowGraph> cfg;

    llvm::LLVMContext *context;
    llvm::Module *module;
    llvm::IRBuilder<> *builder;

    std::string res;

    void createTypes();
    void createGlobals();
    void createPrototypes();
//    void createFunctions();

    llvm::Type* getTypeFromIR(IR_Type const &ir_type);
    llvm::Constant* getConstantFromIR(IRval const &val);

public:
    IR2LLVM(std::shared_ptr<ControlFlowGraph> cfg);

    std::string getRes() const;
};


#endif /* __IR_2_LLVM_HPP__ */
