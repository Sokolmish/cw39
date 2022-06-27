#ifndef IR_NODES_HPP_INCLUDED__
#define IR_NODES_HPP_INCLUDED__

#include <vector>
#include <map>
#include <memory>
#include <optional>
#include <string>

#include "types.hpp"
#include "value.hpp"

#include "parser/common.hpp"


// Expressions

class IR_ExprOper;
class IR_ExprMem;
class IR_ExprAccess;
class IR_ExprAlloc;
class IR_ExprCast;
class IR_ExprCall;
class IR_ExprTerminator;
class IR_ExprPhi;
class IR_ExprNOP;

class IR_Expr {
public:
    std::optional<IRval> res;

    IR_Expr(std::optional<IRval> res);
    virtual ~IR_Expr() = default;

    IR_Expr(IR_Expr&&) noexcept = default;
    IR_Expr& operator=(IR_Expr&&) noexcept = default;

    std::unique_ptr<IR_Expr> copy() const;

    virtual std::vector<IRval*> getArgs() = 0;

    bool isNop() const;

    IR_ExprOper* toOper();
    IR_ExprOper const* toOper() const;
    IR_ExprMem* toMem();
    IR_ExprMem const* toMem() const;
    IR_ExprAccess* toAccess();
    IR_ExprAccess const* toAccess() const;
    IR_ExprAlloc* toAlloc();
    IR_ExprAlloc const* toAlloc() const;
    IR_ExprCast* toCast();
    IR_ExprCast const* toCast() const;
    IR_ExprCall* toCall();
    IR_ExprCall const* toCall() const;
    IR_ExprTerminator* toTerm();
    IR_ExprTerminator const* toTerm() const;
    IR_ExprPhi* toPHI();
    IR_ExprPhi const* toPHI() const;


private:
    virtual std::unique_ptr<IR_Expr> copyBody() const = 0;
};

class IR_ExprOper final : public IR_Expr {
public:
    enum IR_Ops {
        MUL, DIV, REM, ADD, SUB, SHR, SHL,
        XOR, AND, OR, LAND, LOR,
        EQ, NE, GT, LT, GE, LE,
        MOV,
        INTR_CTZ, INTR_CLZ, INTR_POPCNT, INTR_BITREV,
    };

    IR_Ops op;
    std::vector<IRval> args;

    IR_ExprOper(std::optional<IRval> res, IR_Ops op, std::vector<IRval> args);

    std::unique_ptr<IR_ExprOper> copyTyped() const;

    std::vector<IRval*> getArgs() override;

    std::string to_string() const;

private:
    std::unique_ptr<IR_Expr> copyBody() const override;
};

class IR_ExprMem final : public IR_Expr {
public:
    enum MemOps { LOAD, STORE };

    MemOps op;
    IRval addr;
    std::optional<IRval> val;

    IR_ExprMem(std::optional<IRval> res, MemOps op, IRval ptr);
    IR_ExprMem(std::optional<IRval> res, MemOps op, IRval ptr, IRval val);

    std::unique_ptr<IR_ExprMem> copyTyped() const;

    std::vector<IRval*> getArgs() override;

    std::string to_string() const;

private:
    std::unique_ptr<IR_Expr> copyBody() const override;
};

class IR_ExprAccess final : public IR_Expr {
public:
    enum AccessOps { EXTRACT, INSERT, GEP };

    AccessOps op;
    IRval base;
    std::vector<IRval> indices;
    std::optional<IRval> val = std::nullopt;

    IR_ExprAccess(std::optional<IRval> res, AccessOps op, IRval base, std::vector<IRval> ind);
    IR_ExprAccess(std::optional<IRval> res, AccessOps op, IRval base, IRval val, std::vector<IRval> ind);

    std::unique_ptr<IR_ExprAccess> copyTyped() const;

    std::vector<IRval*> getArgs() override;

    std::string to_string() const;

private:
    std::unique_ptr<IR_Expr> copyBody() const override;
};

class IR_ExprAlloc final : public IR_Expr {
public:
    std::shared_ptr<IR_Type> type;
    size_t size;
    bool isOnHeap = false;

    IR_ExprAlloc(std::optional<IRval> res, std::shared_ptr<IR_Type> type, size_t size);
    IR_ExprAlloc(std::optional<IRval> res, std::shared_ptr<IR_Type> type, size_t size, bool onHeap);

    std::unique_ptr<IR_ExprAlloc> copyTyped() const;

    std::vector<IRval*> getArgs() override;

    std::string to_string() const;

private:
    std::unique_ptr<IR_Expr> copyBody() const override;
};

class IR_ExprCast final : public IR_Expr {
public:
    IRval arg;
    std::shared_ptr<IR_Type> dest;

    enum CastType {
        BITCAST, SEXT, ZEXT, TRUNC,
        FPTOUI, FPTOSI, UITOFP, SITOFP,
        PTRTOI, ITOPTR,
        FPEXT, FPTRUNC,
    } castOp;

    IR_ExprCast(std::optional<IRval> res, IRval sourceVal, std::shared_ptr<IR_Type> dest);

    std::unique_ptr<IR_ExprCast> copyTyped() const;

    std::vector<IRval*> getArgs() override;

    std::string to_string() const;

private:
    std::unique_ptr<IR_Expr> copyBody() const override;
};

class IR_ExprCall final : public IR_Expr {
public:
    std::variant<int, IRval> callee;
    std::vector<IRval> args;

    IR_ExprCall(std::optional<IRval> res, int callee, std::vector<IRval> args);
    IR_ExprCall(std::optional<IRval> res, IRval callee, std::vector<IRval> args);

    std::vector<IRval*> getArgs() override;

    std::unique_ptr<IR_ExprCall> copyTyped() const;

    bool isIndirect() const;
    int getFuncId() const;
    IRval getFuncPtr() const;

private:
    std::unique_ptr<IR_Expr> copyBody() const override;
};

class IR_ExprTerminator final : public IR_Expr {
public:
    enum TermType { RET, BRANCH, JUMP } termType;
    std::optional<IRval> arg;

    explicit IR_ExprTerminator(TermType type);
    IR_ExprTerminator(TermType type, IRval val);

    std::unique_ptr<IR_ExprTerminator> copyTyped() const;

    std::vector<IRval*> getArgs() override;

private:
    std::unique_ptr<IR_Expr> copyBody() const override;
};

class IR_ExprPhi final : public IR_Expr {
public:
    /** Key is position of argument */
    std::map<int, IRval> args;

    IR_ExprPhi(std::optional<IRval> res);

    std::unique_ptr<IR_ExprPhi> copyTyped() const;

    std::vector<IRval*> getArgs() override;

private:
    std::unique_ptr<IR_Expr> copyBody() const override;
};


class IR_ExprNOP final : public IR_Expr {
public:
    IR_ExprNOP();
    std::vector<IRval*> getArgs() override;

private:
    std::unique_ptr<IR_Expr> copyBody() const override;
};


// Casts

bool IR_Expr::isNop() const {
    return bool(dynamic_cast<IR_ExprNOP const *>(this));
}

#define CREATE_IR_EXPR_CASTER(expr_type, name) \
    expr_type* IR_Expr::to##name() { return dynamic_cast<expr_type *>(this); } \
    expr_type const* IR_Expr::to##name() const { return dynamic_cast<expr_type const *>(this); }

CREATE_IR_EXPR_CASTER(IR_ExprPhi, PHI)
CREATE_IR_EXPR_CASTER(IR_ExprOper, Oper)
CREATE_IR_EXPR_CASTER(IR_ExprMem, Mem)
CREATE_IR_EXPR_CASTER(IR_ExprAccess, Access)
CREATE_IR_EXPR_CASTER(IR_ExprAlloc, Alloc)
CREATE_IR_EXPR_CASTER(IR_ExprCast, Cast)
CREATE_IR_EXPR_CASTER(IR_ExprCall, Call)
CREATE_IR_EXPR_CASTER(IR_ExprTerminator, Term)

#undef CREATE_IR_EXPR_CASTER


// Blocks

class IR_Block {
public:
    int id;

    std::vector<std::unique_ptr<IR_Expr>> phis; // Base calss because of NOPs
    std::vector<std::unique_ptr<IR_Expr>> body;

    std::unique_ptr<IR_ExprTerminator> termNode;

    std::vector<int> prev;
    std::vector<int> next;

    explicit IR_Block(int id);
    IR_Block copy() const;

    void addNode(std::unique_ptr<IR_Expr> expr);

    IR_ExprPhi& addNewPhiNode(IRval res);

    void setTerminator(IR_ExprTerminator::TermType type);
    void setTerminator(IR_ExprTerminator::TermType type, IRval arg);

    std::vector<IRval> getDefinitions() const;
    std::vector<IRval> getReferences() const;

    IR_ExprTerminator const* getTerminator() const;

    std::vector<IR_Expr*> getAllNodes();

    void removePredecessor(int pred);
};

#endif /* IR_NODES_HPP_INCLUDED__ */
