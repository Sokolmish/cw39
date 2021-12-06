#ifndef __IR_ELEM_HPP__
#define __IR_ELEM_HPP__

#include <vector>
#include <memory>
#include <array>
#include <optional>
#include <variant>
#include <string>


// Types

struct IR_Type {
    enum Type { DIRECT, POINTER, ARRAY, FUNCTION } type;

    explicit IR_Type(Type type);
    virtual ~IR_Type() = default;
    virtual bool equal(IR_Type const &rhs) const = 0;
};

struct IR_TypeDirect : public IR_Type {
    enum DirType {
        VOID, I8, U8, I32, U32, I64, U64, F32
    } spec;

    explicit IR_TypeDirect(DirType spec);
    bool equal(IR_Type const &rhs) const override;

    [[nodiscard]] bool isInteger() const;
    [[nodiscard]] bool isFloat() const;
    [[nodiscard]] bool isSigned() const;
    [[nodiscard]] bool isUnsigned() const;
    [[nodiscard]] int getBytesSize() const;
};

struct IR_TypePtr : public IR_Type {
    std::shared_ptr<IR_Type> child;
    bool is_const = false;
    bool is_restrict = false;
    bool is_volatile = false;

    explicit IR_TypePtr(std::shared_ptr<IR_Type> child);
    bool equal(IR_Type const &rhs) const override;
};

struct IR_TypeArray : public IR_Type {
    std::shared_ptr<IR_Type> child;
    uint64_t size;

    IR_TypeArray(std::shared_ptr<IR_Type> child, uint64_t size);
    bool equal(IR_Type const &rhs) const override;
};

struct IR_TypeFunc : public IR_Type {
    std::shared_ptr<IR_Type> ret;
    std::vector<std::shared_ptr<IR_Type>> args;
    bool isVariadic;

    explicit IR_TypeFunc(std::shared_ptr<IR_Type> ret);
    IR_TypeFunc(std::shared_ptr<IR_Type> ret, std::vector<std::shared_ptr<IR_Type>> args, bool variadic);
    bool equal(IR_Type const &rhs) const override;
};


// Values

class IRval {
public:
    typedef std::variant<uint64_t, float> union_type;

private:
    static uint64_t regs_counter;

    std::shared_ptr<IR_Type> type;
    bool isReg;
    bool isTemp = false; // TODO
    union_type val;

public:
    IRval(std::shared_ptr<IR_Type> type, bool isReg, union_type v);

    [[nodiscard]] static IRval createVal(std::shared_ptr<IR_Type> type, union_type v);
    [[nodiscard]] static IRval createReg(std::shared_ptr<IR_Type> type);

    [[nodiscard]] std::shared_ptr<IR_Type> const& getType() const;
    [[nodiscard]] bool isConstant() const;
    [[nodiscard]] bool isRegister() const;
    [[nodiscard]] union_type const& getVal() const;

    [[nodiscard]] std::string to_string() const;

    template <class T>
    [[nodiscard]] T castValTo() const {
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
    IR_DEREF,
    IR_ALLOCA, IR_STORE,
};

struct IR_Expr {
    enum Type { OPERATION, STACKOP, CALL } type;

    explicit IR_Expr(Type type);
    virtual ~IR_Expr() = default;
};

struct IR_ExprOper : public IR_Expr {
    IR_Ops op;
    std::vector<IRval> args;

    IR_ExprOper(IR_Ops op, std::vector<IRval> args);

    std::string opToString() const;
};

struct IR_ExprCall : public IR_Expr {

};


// Nodes

struct IR_Node {
    std::optional<IRval> res;
    std::unique_ptr<IR_Expr> body;

    IR_Node(IRval res, std::unique_ptr<IR_Expr> body);
    IR_Node(std::unique_ptr<IR_Expr> body);
};

struct IR_Terminator {
    enum TermType { NONE, RET, BRANCH, JUMP } type;
    std::optional<IRval> val;

    IR_Terminator();
    IR_Terminator(TermType type);
    IR_Terminator(TermType type, IRval val);
};

// Blocks

struct IR_Block {
    int id;
    std::vector<IR_Node> body;

    std::vector<IR_Block*> prev;
    std::vector<IR_Block*> next;
    IR_Terminator terminator;

    explicit IR_Block(int id);
    void addNode(IR_Node node);
};

#endif /* __IR_ELEM_HPP__ */
