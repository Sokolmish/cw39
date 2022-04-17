#ifndef IR_2_LLVM_HPP_INCLUDED__
#define IR_2_LLVM_HPP_INCLUDED__

#include <memory>
#include "ir/unit.hpp"


class IR2LLVM {
public:
    explicit IR2LLVM(IntermediateUnit const &iunit);

    IR2LLVM(IR2LLVM const&) = delete;
    IR2LLVM& operator=(IR2LLVM const&) = delete;

    [[nodiscard]] std::string getRes() const;

    ~IR2LLVM(); // Needed for unique_ptr to incomplete type

    class IR2LLVM_Impl;

private:
    friend class IR2LLVM_Impl;
    std::unique_ptr<IR2LLVM_Impl> impl;

    IntermediateUnit const &iunit;
    std::string llvmIR;
};


#endif /* IR_2_LLVM_HPP_INCLUDED__ */
