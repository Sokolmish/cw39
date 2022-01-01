#ifndef __IR_2_LLVM_HPP__
#define __IR_2_LLVM_HPP__

#include <memory>
#include "ir/cfg.hpp"


class IR2LLVM {
public:
    IR2LLVM(std::shared_ptr<ControlFlowGraph> cfg);

    IR2LLVM(IR2LLVM const&) = delete;
    IR2LLVM& operator=(IR2LLVM const&) = delete;

    std::string getRes() const;

    ~IR2LLVM(); // Needed for unique_ptr to incomplete type

    class IR2LLVM_Impl;

private:
    friend class IR2LLVM_Impl;
    std::unique_ptr<IR2LLVM_Impl> impl;

    std::shared_ptr<ControlFlowGraph> cfg;
    std::string llvmIR = "";
};


#endif /* __IR_2_LLVM_HPP__ */
