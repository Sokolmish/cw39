#ifndef __IR_ELEM_HPP__
#define __IR_ELEM_HPP__

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
struct IR_ExprAlloc;
struct IR_ExprCast;
struct IR_ExprCall;
struct IR_ExprTerminator;
struct IR_ExprPhi;

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
    // TODO: move memory operations into separate category
    enum IR_Ops {
        MUL, DIV, REM, ADD, SUB, SHR, SHL,
        XOR, AND, OR, LAND, LOR,
        EQ, NE, GT, LT, GE, LE,
        LOAD, STORE, EXTRACT, INSERT, MOV // GEP
    };

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

struct IR_ExprTerminator : public IR_Expr {
    enum TermType { RET, BRANCH, JUMP } termType;
    std::optional<IRval> arg;

    explicit IR_ExprTerminator(TermType type);
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
    IR_Block copy() const;

    void addNode(IR_Node node);
    void addNode(std::optional<IRval> res, std::unique_ptr<IR_Expr> expr);

    void addOperNode(std::optional<IRval> res, IR_ExprOper::IR_Ops op, std::vector<IRval> args);
    void addCastNode(IRval res, IRval sourceVal, std::shared_ptr<IR_Type> dest);
    void addCallNode(std::optional<IRval> res, int callee, std::vector<IRval> args);
    void addIndirectCallNode(std::optional<IRval> res, IRval callee, std::vector<IRval> args);
    void addAllocNode(IRval res, std::shared_ptr<IR_Type> type, bool isOnHeap = false);

    void addNewPhiNode(IRval res);

    void setTerminator(IR_ExprTerminator::TermType type);
    void setTerminator(IR_ExprTerminator::TermType type, IRval arg);

    std::vector<IRval> getDefinitions() const;
    std::vector<IRval> getReferences() const;

    IR_ExprTerminator const* getTerminator() const;

    std::vector<IR_Node*> getAllNodes();
};

#endif /* __IR_ELEM_HPP__ */
