#ifndef __CONSTANTS_FOLDER_HPP__
#define __CONSTANTS_FOLDER_HPP__

#include "value.hpp"
#include "nodes.hpp"
#include <optional>

class ConstantsFolder {
public:
    static std::optional<IRval> foldExpr(IR_Expr const &expr);

    static constexpr bool isGeneralNumOp(IR_ExprOper::IR_Ops op);
    static constexpr bool isIntegerOp(IR_ExprOper::IR_Ops op);
    static constexpr bool isComparsionOp(IR_ExprOper::IR_Ops op);
    static constexpr bool isShortLogicOp(IR_ExprOper::IR_Ops op);

private:
    static std::optional<IRval> foldOper(IR_ExprOper const &expr);
    static std::optional<IRval> foldCast(IR_ExprCast const &expr);
    static std::optional<IRval> foldPhi(IR_ExprPhi const &expr);

};

#endif /* __CONSTANTS_FOLDER_HPP__ */
