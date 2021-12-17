#ifndef __IR_ELEM_HPP__
#define __IR_ELEM_HPP__

#include <vector>
#include <memory>
#include <optional>
#include <variant>
#include <string>
#include "types.hpp"
#include "parser/common.h"

// Values

class IRval {
public:
    typedef std::variant<
            uint8_t, int8_t, uint32_t, int32_t, uint64_t, int64_t,
            float, double> union_type;

private:
    std::shared_ptr<IR_Type> type;
    enum ValueClass { VAL, VREG, FUN_PARAM, GLOBAL } valClass;
    union_type val;

public:
    IRval(std::shared_ptr<IR_Type> type, ValueClass vclass, union_type v);
    IRval copy() const;

    bool operator==(IRval const &oth) const;

    static bool less(const IRval &a, const IRval &b);
    struct Comparator {
        bool operator()(const IRval& a, const IRval& b) const;
    };

    [[nodiscard]] static IRval createVal(std::shared_ptr<IR_Type> type, union_type v);
    [[nodiscard]] static IRval createReg(std::shared_ptr<IR_Type> type, uint64_t id);
    [[nodiscard]] static IRval createFunArg(std::shared_ptr<IR_Type> type, uint64_t num);

    std::shared_ptr<IR_Type> const& getType() const;
    bool isConstant() const;
    bool isRegister() const;
    bool isGlobal() const;
    union_type const& getVal() const;
    std::string to_string() const;

    template <class T>
    T castValTo() const {
        return std::visit([](auto const &arg) -> T {
            return static_cast<T>(arg);
        }, val);
    }
};


// Expressions

enum IR_Ops {
    IR_MUL, IR_DIV, IR_REM, IR_ADD, IR_SUB, IR_SHR, IR_SHL,
    IR_XOR, IR_AND, IR_OR, IR_LAND, IR_LOR,
    IR_EQ, IR_NE, IR_GT, IR_LT, IR_GE, IR_LE,
    IR_LOAD, IR_STORE, IR_EXTRACT, IR_INSERT, IR_MOV // IR_GEP
};

struct IR_Expr {
    enum Type { OPERATION, ALLOCATION, CAST, CALL, PHI } type;

    explicit IR_Expr(Type type);
    virtual ~IR_Expr() = default;
    virtual std::unique_ptr<IR_Expr> copy() const = 0;
    virtual std::vector<IRval*> getArgs() = 0;
};

struct IR_ExprOper : public IR_Expr {
    IR_Ops op;
    std::vector<IRval> args;

    IR_ExprOper(IR_Ops op, std::vector<IRval> args);
    std::unique_ptr<IR_Expr> copy() const override;
    std::vector<IRval*> getArgs() override;

    std::string opToString() const;
};

struct IR_ExprAlloc : public IR_Expr {
    std::shared_ptr<IR_Type> type;
    size_t size;
    bool isOnHeap = false;

    IR_ExprAlloc(std::shared_ptr<IR_Type> type, size_t size);
    IR_ExprAlloc(std::shared_ptr<IR_Type> type, size_t size, bool onHeap);
    std::unique_ptr<IR_Expr> copy() const override;
    std::vector<IRval*> getArgs() override;

    std::string opToString() const;
};

struct IR_ExprCast : public IR_Expr {
    IRval arg;
    std::shared_ptr<IR_Type> dest;

    enum CastType {
        BITCAST, SEXT, ZEXT, TRUNC,
        FPTOUI, FPTOSI, UITOFP, SITOFP,
        PTRTOI, ITOPTR
    } castOp;

    IR_ExprCast(IRval sourceVal, std::shared_ptr<IR_Type> dest);
    std::unique_ptr<IR_Expr> copy() const override;
    std::vector<IRval*> getArgs() override;

    std::string opToString() const;
};

struct IR_ExprCall : public IR_Expr {
    int funcId;
    std::vector<IRval> args;

    IR_ExprCall(int callee, std::vector<IRval> args);
    std::unique_ptr<IR_Expr> copy() const override;
    std::vector<IRval*> getArgs() override;
};


// Nodes

struct IR_Node {
    std::optional<IRval> res;
    std::unique_ptr<IR_Expr> body;

    IR_Node(IRval res, std::unique_ptr<IR_Expr> body);
    explicit IR_Node(std::unique_ptr<IR_Expr> body);
    IR_Node copy() const;
};

struct IR_Terminator {
    enum TermType { NONE, RET, BRANCH, JUMP } type;
    std::optional<IRval> arg;

    IR_Terminator();
    IR_Terminator(TermType type);
    IR_Terminator(TermType type, IRval val);
    bool exist() const;
    IR_Terminator copy() const;
};


// Blocks

class IR_Block {
public:
    int id;
    std::vector<IR_Node> body;

    std::vector<int> prev;
    std::vector<int> next;
    IR_Terminator terminator;

    explicit IR_Block(int id);
    void addNode(IR_Node node);
    IR_Block copy() const;
};

#endif /* __IR_ELEM_HPP__ */
