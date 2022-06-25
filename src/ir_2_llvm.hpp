#ifndef IR_2_LLVM_HPP_INCLUDED__
#define IR_2_LLVM_HPP_INCLUDED__

#include <memory>
#include "ir/unit.hpp"


class IR2LLVM {
public:
    explicit IR2LLVM(IntermediateUnit const &iunit);

    IR2LLVM(IR2LLVM const&) = delete;
    IR2LLVM& operator=(IR2LLVM const&) = delete;

    std::string const& getLLVM_IR() const;
    std::vector<char> const& getLLVM_BC() const;

    auto moveData() && {
        return std::make_tuple(
                std::move(llvmIR),
                std::move(llvmBC));
    }

    ~IR2LLVM(); // Needed for unique_ptr to incomplete type

    class IR2LLVM_Impl;

private:
    friend class IR2LLVM_Impl;
    std::unique_ptr<IR2LLVM_Impl> impl;

    IntermediateUnit const &iunit;
    std::string llvmIR;
    std::vector<char> llvmBC;
};

#endif /* IR_2_LLVM_HPP_INCLUDED__ */
