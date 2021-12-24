#ifndef __IR_ELEM_HPP__
#define __IR_ELEM_HPP__

#include <vector>
#include <map>
#include <memory>
#include <optional>
#include <variant>
#include <string>
#include "types.hpp"
#include "parser/common.hpp"

// Values

class IRval {
public:
    typedef std::variant<
            uint8_t, int8_t, uint32_t, int32_t, uint64_t, int64_t,
            float, double> union_type;
    enum ValueClass { VAL, VREG, GLOBAL, STRING, FUN_PARAM } valClass;

private:
    std::shared_ptr<IR_Type> type;
    union_type val;

public:
    std::optional<int> version = {};

    IRval(std::shared_ptr<IR_Type> type, ValueClass vclass, union_type v);
    IRval copy() const;

    bool operator==(IRval const &oth) const;

    static bool less(const IRval &a, const IRval &b);
    struct Comparator {
        bool operator()(const IRval& a, const IRval& b) const;
    };

    static bool lessVersions(const IRval &a, const IRval &b);
    struct ComparatorVersions {
        bool operator()(const IRval& a, const IRval& b) const;
    };

    [[nodiscard]] static IRval createVal(std::shared_ptr<IR_Type> type, union_type v);
    [[nodiscard]] static IRval createReg(std::shared_ptr<IR_Type> type, uint64_t id);
    [[nodiscard]] static IRval createFunArg(std::shared_ptr<IR_Type> type, uint64_t num);
    [[nodiscard]] static IRval createString(uint64_t num);
    [[nodiscard]] static IRval createGlobal(std::shared_ptr<IR_Type> globalType, uint64_t num);
    [[nodiscard]] static IRval createDefault(std::shared_ptr<IR_Type> type);

    std::shared_ptr<IR_Type> const& getType() const;
    bool isConstant() const;
    bool isVReg() const;
    bool isGlobal() const;
    union_type const& getVal() const;
    std::string to_string() const;
    std::string to_reg_name() const;

    template <class T>
    T castValTo() const {
        return std::visit([](auto const &arg) -> T {
            return static_cast<T>(arg);
        }, val);
    }
};


// Expressions

struct IR_ExprOper;
struct IR_ExprAlloc;
struct IR_ExprCast;
struct IR_ExprCall;
struct IR_ExprTerminator;
struct IR_ExprPhi;

enum IR_Ops {
    IR_MUL, IR_DIV, IR_REM, IR_ADD, IR_SUB, IR_SHR, IR_SHL,
    IR_XOR, IR_AND, IR_OR, IR_LAND, IR_LOR,
    IR_EQ, IR_NE, IR_GT, IR_LT, IR_GE, IR_LE,
    IR_LOAD, IR_STORE, IR_EXTRACT, IR_INSERT, IR_MOV // IR_GEP
};

struct IR_Expr {
    enum Type { OPERATION, ALLOCATION, CAST, CALL, TERM, PHI } type;

    explicit IR_Expr(Type type);
    virtual ~IR_Expr() = default;
    virtual std::unique_ptr<IR_Expr> copy() const = 0;
    virtual std::vector<IRval*> getArgs() = 0;

    IR_ExprOper const& getOper() const;
    IR_ExprAlloc const& getAlloc() const;
    IR_ExprCast const& getCast() const;
    IR_ExprCall const& getCall() const;
    IR_ExprTerminator const& getTerm() const;
    IR_ExprPhi const& getPhi() const;
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
        PTRTOI, ITOPTR,
        FPEXT, FPTRUNC,
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

struct IR_ExprTerminator : public IR_Expr {
    enum TermType { RET, BRANCH, JUMP } termType;
    std::optional<IRval> arg;

    IR_ExprTerminator(TermType type);
    IR_ExprTerminator(TermType type, IRval val);
    std::unique_ptr<IR_Expr> copy() const override;
    std::vector<IRval*> getArgs() override;
};

struct IR_ExprPhi : public IR_Expr {
    /** Key is position of argument */
    std::map<int, IRval> args;

    IR_ExprPhi();
    std::unique_ptr<IR_Expr> copy() const override;
    std::vector<IRval*> getArgs() override;
};


// Nodes

struct IR_Node {
    std::optional<IRval> res;
    std::unique_ptr<IR_Expr> body;

    explicit IR_Node(std::unique_ptr<IR_Expr> body);
    IR_Node(IRval res, std::unique_ptr<IR_Expr> body);
    IR_Node copy() const;
};


// Blocks

class IR_Block {
public:
    int id;

    std::vector<IR_Node> phis;
    std::vector<IR_Node> body;
    std::optional<IR_Node> termNode;

    std::vector<int> prev;
    std::vector<int> next;

    explicit IR_Block(int id);
    void addNode(IR_Node node);
    IR_Block copy() const;

    std::vector<IRval> getDefinitions() const;
    std::vector<IRval> getReferences() const;

    IR_ExprTerminator const* getTerminator() const;

    std::vector<IR_Node*> getAllNodes();
};

#endif /* __IR_ELEM_HPP__ */
