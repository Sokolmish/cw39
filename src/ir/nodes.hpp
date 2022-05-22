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

struct IR_ExprOper;
struct IR_ExprMem;
struct IR_ExprAccess;
struct IR_ExprAlloc;
struct IR_ExprCast;
struct IR_ExprCall;
struct IR_ExprTerminator;
struct IR_ExprPhi;

struct IR_Expr {
    enum Type {
        OPERATION, MEMORY, ACCESS, ALLOCATION, CAST, CALL, TERM, PHI
    };
    Type type;

    explicit IR_Expr(Type type);
    virtual ~IR_Expr() = default;
    virtual std::unique_ptr<IR_Expr> copy() const = 0;
    virtual std::vector<IRval*> getArgs() = 0;

    IR_ExprOper const& getOper() const;
    IR_ExprMem const& getMem() const;
    IR_ExprAccess const& getAccess() const;
    IR_ExprAlloc const& getAlloc() const;
    IR_ExprCast const& getCast() const;
    IR_ExprCall const& getCall() const;
    IR_ExprTerminator const& getTerm() const;
    IR_ExprPhi const& getPhi() const;
};

struct IR_ExprOper final : public IR_Expr {
    enum IR_Ops {
        MUL, DIV, REM, ADD, SUB, SHR, SHL,
        XOR, AND, OR, LAND, LOR,
        EQ, NE, GT, LT, GE, LE,
        MOV,
        INTR_CTZ, INTR_CLZ, INTR_POPCNT, INTR_BITREV,
    };

    IR_Ops op;
    std::vector<IRval> args;

    IR_ExprOper(IR_Ops op, std::vector<IRval> args);
    std::unique_ptr<IR_Expr> copy() const override;
    std::vector<IRval*> getArgs() override;
    std::string to_string() const;
};

struct IR_ExprMem final : public IR_Expr {
    enum MemOps { LOAD, STORE };

    MemOps op;
    IRval addr;
    std::optional<IRval> val;

    IR_ExprMem(MemOps op, IRval ptr);
    IR_ExprMem(MemOps op, IRval ptr, IRval val);
    std::unique_ptr<IR_Expr> copy() const override;
    std::vector<IRval*> getArgs() override;
    std::string to_string() const;
};

struct IR_ExprAccess final : public IR_Expr {
    enum AccessOps { EXTRACT, INSERT, GEP };

    AccessOps op;
    IRval base;
    std::vector<IRval> indices;
    std::optional<IRval> val = {};

    IR_ExprAccess(AccessOps op, IRval base, std::vector<IRval> ind);
    IR_ExprAccess(AccessOps op, IRval base, IRval val, std::vector<IRval> ind);
    std::unique_ptr<IR_Expr> copy() const override;
    std::vector<IRval*> getArgs() override;
    std::string to_string() const;
};

struct IR_ExprAlloc final : public IR_Expr {
    std::shared_ptr<IR_Type> type;
    size_t size;
    bool isOnHeap = false;

    IR_ExprAlloc(std::shared_ptr<IR_Type> type, size_t size);
    IR_ExprAlloc(std::shared_ptr<IR_Type> type, size_t size, bool onHeap);
    std::unique_ptr<IR_Expr> copy() const override;
    std::vector<IRval*> getArgs() override;
    std::string to_string() const;
};

struct IR_ExprCast final : public IR_Expr {
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
    std::string to_string() const;
};

struct IR_ExprCall final : public IR_Expr {
    std::variant<int, IRval> callee;
    std::vector<IRval> args;

    IR_ExprCall(int callee, std::vector<IRval> args);
    IR_ExprCall(IRval callee, std::vector<IRval> args);

    std::unique_ptr<IR_Expr> copy() const override;
    std::vector<IRval*> getArgs() override;

    bool isIndirect() const;
    int getFuncId() const;
    IRval getFuncPtr() const;
};

struct IR_ExprTerminator final : public IR_Expr {
    enum TermType { RET, BRANCH, JUMP } termType;
    std::optional<IRval> arg;

    explicit IR_ExprTerminator(TermType type);
    IR_ExprTerminator(TermType type, IRval val);
    std::unique_ptr<IR_Expr> copy() const override;
    std::vector<IRval*> getArgs() override;
};

struct IR_ExprPhi final : public IR_Expr {
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

    static IR_Node nop();
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
    IR_Block copy() const;

    void addNode(IR_Node node);
    void addNode(std::optional<IRval> res, std::unique_ptr<IR_Expr> expr);

    IR_Node& addNewPhiNode(IRval res);

    void setTerminator(IR_ExprTerminator::TermType type);
    void setTerminator(IR_ExprTerminator::TermType type, IRval arg);

    std::vector<IRval> getDefinitions() const;
    std::vector<IRval> getReferences() const;

    IR_ExprTerminator const* getTerminator() const;

    std::vector<IR_Node*> getAllNodes();
};

#endif /* IR_NODES_HPP_INCLUDED__ */
