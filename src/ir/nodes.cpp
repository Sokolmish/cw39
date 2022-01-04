#include "nodes.hpp"
#include "utils.hpp"

// Expressions

IR_Expr::IR_Expr(IR_Expr::Type type) : type(type) {}

IR_ExprOper const &IR_Expr::getOper() const {
    return dynamic_cast<IR_ExprOper const&>(*this);
}

IR_ExprAlloc const &IR_Expr::getAlloc() const {
    return dynamic_cast<IR_ExprAlloc const&>(*this);
}

IR_ExprCast const &IR_Expr::getCast() const {
    return dynamic_cast<IR_ExprCast const&>(*this);
}

IR_ExprCall const &IR_Expr::getCall() const {
    return dynamic_cast<IR_ExprCall const&>(*this);
}

IR_ExprTerminator const &IR_Expr::getTerm() const {
    return dynamic_cast<IR_ExprTerminator const&>(*this);
}

IR_ExprPhi const& IR_Expr::getPhi() const {
    return dynamic_cast<IR_ExprPhi const&>(*this);
}


IR_ExprOper::IR_ExprOper(IR_Ops op, std::vector<IRval> args) :
        IR_Expr(IR_Expr::OPERATION), op(op), args(std::move(args)) {
    // TODO: check args count
}

std::unique_ptr<IR_Expr> IR_ExprOper::copy() const {
    std::vector<IRval> newArgs;
    for (auto const &arg : args)
        newArgs.push_back(arg.copy());
    return std::make_unique<IR_ExprOper>(op, std::move(newArgs));
}

std::vector<IRval*> IR_ExprOper::getArgs() {
    std::vector<IRval*> resArgs;
    for (auto &arg : args)
        resArgs.push_back(&arg);
    return resArgs;
}

std::string IR_ExprOper::opToString() const {
    switch (op) {
        case MUL:        return "mul";
        case DIV:        return "div";
        case REM:        return "rem";
        case ADD:        return "add";
        case SUB:        return "sub";
        case SHR:        return "shr";
        case SHL:        return "shl";
        case XOR:        return "xor";
        case AND:        return "and";
        case OR:         return "or";
        case LAND:       return "land";
        case LOR:        return "lor";
        case EQ:         return "eq";
        case NE:         return "ne";
        case GT:         return "gt";
        case LT:         return "lt";
        case GE:         return "ge";
        case LE:         return "le";
        case LOAD:       return "load";
        case STORE:      return "store";
        case EXTRACT:    return "extract";
        case INSERT:     return "insert";
        case MOV:        return "mov";
//        case IR_GEP:        return "gep";
    }
    throw;
}


// IR_ExprAlloc

IR_ExprAlloc::IR_ExprAlloc(std::shared_ptr<IR_Type> type, size_t size) :
        IR_Expr(ALLOCATION), type(std::move(type)), size(size) {}

IR_ExprAlloc::IR_ExprAlloc(std::shared_ptr<IR_Type> type, size_t size, bool onHeap) :
        IR_Expr(ALLOCATION), type(std::move(type)), size(size), isOnHeap(onHeap) {}

std::unique_ptr<IR_Expr> IR_ExprAlloc::copy() const {
    return std::make_unique<IR_ExprAlloc>(type->copy(), size, isOnHeap);
}

std::string IR_ExprAlloc::opToString() const {
    return isOnHeap ? "malloc" : "alloca";
}

std::vector<IRval*> IR_ExprAlloc::getArgs() {
    return {};
}


// IR_ExprCast

IR_ExprCast::IR_ExprCast(IRval sourceVal, std::shared_ptr<IR_Type> cdest)
        : IR_Expr(CAST), arg(std::move(sourceVal)), dest(std::move(cdest)) {
    const auto &source = arg.getType();
    if (source->equal(*dest))
        semanticError("Casting equal types"); // TODO

    if (source->type == IR_Type::FUNCTION || dest->type == IR_Type::FUNCTION) {
        semanticError("Function type cannot be cast"); // TODO
    }
    else if (source->type == IR_Type::ARRAY) {
        if (dest->type != IR_Type::POINTER)
            semanticError("Array type can be cast only to pointer");
        castOp = BITCAST;
    }
    else if (source->type == IR_Type::POINTER && dest->type == IR_Type::POINTER) {
        castOp = BITCAST;
    }
    else if (source->type == IR_Type::POINTER && dest->type == IR_Type::DIRECT) {
        auto const &dstDir = dynamic_cast<IR_TypeDirect const &>(*dest);
        if (!dstDir.isInteger() || dstDir.spec != IR_TypeDirect::U64)
            semanticError("Pointer can be cast only to u64 number");
        castOp = PTRTOI;
    }
    else if (source->type == IR_Type::DIRECT && dest->type == IR_Type::POINTER) {
        auto const &srcDir = dynamic_cast<IR_TypeDirect const &>(*source);
        if (!srcDir.isInteger() || srcDir.spec != IR_TypeDirect::U64)
            semanticError("Pointer can be created only from u64 number");
        castOp = ITOPTR;
    }
    else if (source->type == IR_Type::DIRECT && dest->type == IR_Type::DIRECT) {
        auto const &srcDir = dynamic_cast<IR_TypeDirect const &>(*source);
        auto const &dstDir = dynamic_cast<IR_TypeDirect const &>(*dest);

        if (dstDir.spec == IR_TypeDirect::VOID)
            semanticError("Casts to void are not allowed");

        if (srcDir.isInteger() && dstDir.isInteger()) {
            if (srcDir.getBytesSize() == dstDir.getBytesSize())
                castOp = BITCAST;
            else if (srcDir.getBytesSize() > dstDir.getBytesSize())
                castOp = TRUNC;
            else if (srcDir.isSigned() && dstDir.isSigned())
                castOp = SEXT;
            else
                castOp = ZEXT;
        }
        else if (srcDir.isFloat() && dstDir.isFloat()) {
            if (srcDir.getBytesSize() == 8 && dstDir.getBytesSize() == 4)
                castOp = FPTRUNC;
            else if (srcDir.getBytesSize() == 4 && dstDir.getBytesSize() == 8)
                castOp = FPEXT;
            else
                internalError("Wrong float conversion");
        }
        else if (srcDir.isInteger() && dstDir.isFloat()) {
            if (srcDir.isSigned())
                castOp = SITOFP;
            else
                castOp = UITOFP;
        }
        else if (srcDir.isFloat() && dstDir.isInteger()) {
            if (dstDir.isSigned())
                castOp = FPTOSI;
            else
                castOp = FPTOUI;
        }
    }
}

std::unique_ptr<IR_Expr> IR_ExprCast::copy() const {
    return std::make_unique<IR_ExprCast>(arg.copy(), dest->copy());
}

std::vector<IRval*> IR_ExprCast::getArgs() {
    return std::vector<IRval*>{ &arg };
}

std::string IR_ExprCast::opToString() const {
    switch (castOp) {
        case BITCAST:   return "bitcast";
        case SEXT:      return "sext";
        case ZEXT:      return "zext";
        case TRUNC:     return "trunc";
        case FPTOUI:    return "fp_to_ui";
        case FPTOSI:    return "fp_to_si";
        case UITOFP:    return "ui_to_fp";
        case SITOFP:    return "si_to_fp";
        case PTRTOI:    return "ptrtoi";
        case ITOPTR:    return "itoptr";
        case FPEXT:     return "fpext";
        case FPTRUNC:   return "fptrunc";
    }
    throw;
}


// IR_ExprCall

IR_ExprCall::IR_ExprCall(int callee, std::vector<IRval> args)
        : IR_Expr(CALL), callee(callee), args(std::move(args)) {}

IR_ExprCall::IR_ExprCall(IRval callee, std::vector<IRval> args)
        : IR_Expr(CALL), callee(callee), args(std::move(args)) {}

std::unique_ptr<IR_Expr> IR_ExprCall::copy() const {
    std::vector<IRval> newArgs;
    for (auto const &arg : args)
        newArgs.push_back(arg.copy());
    if (isIndirect())
        return std::make_unique<IR_ExprCall>(getFuncPtr().copy(), std::move(newArgs));
    else
        return std::make_unique<IR_ExprCall>(getFuncId(), std::move(newArgs));
}

std::vector<IRval*> IR_ExprCall::getArgs() {
    std::vector<IRval*> resArgs;
    if (isIndirect())
        resArgs.push_back(&std::get<IRval>(callee));
    for (auto &arg : args)
        resArgs.push_back(&arg);
    return resArgs;
}

bool IR_ExprCall::isIndirect() const {
    return std::holds_alternative<IRval>(callee);
}

int IR_ExprCall::getFuncId() const {
    return std::get<int>(callee);
}

IRval IR_ExprCall::getFuncPtr() const {
    return std::get<IRval>(callee);
}


// IR_ExprPhi

IR_ExprPhi::IR_ExprPhi() : IR_Expr(PHI) {}

std::unique_ptr<IR_Expr> IR_ExprPhi::copy() const {
    auto res = std::make_unique<IR_ExprPhi>();
    for (auto const &[pos, arg] : args)
        res->args.emplace(pos, arg.copy());
    return res;
}

std::vector<IRval *> IR_ExprPhi::getArgs() {
    std::vector<IRval*> resArgs;
    for (auto &[pos, arg] : args)
        resArgs.push_back(&arg);
    return resArgs;
}


// IR_ExprTerminator

IR_ExprTerminator::IR_ExprTerminator(IR_ExprTerminator::TermType type)
        : IR_Expr(TERM), termType(type), arg() {}

IR_ExprTerminator::IR_ExprTerminator(IR_ExprTerminator::TermType type, IRval val)
        : IR_Expr(TERM), termType(type), arg(std::move(val)) {}

std::unique_ptr<IR_Expr> IR_ExprTerminator::copy() const {
    if (arg.has_value())
        return std::make_unique<IR_ExprTerminator>(termType, arg->copy());
    else
        return std::make_unique<IR_ExprTerminator>(termType);
}

std::vector<IRval *> IR_ExprTerminator::getArgs() {
    if (arg.has_value())
        return std::vector<IRval *>{ &(*arg) };
    else
        return std::vector<IRval *>();
}


// IR_Node

IR_Node::IR_Node(IRval res, std::unique_ptr<IR_Expr> body) : res(res), body(std::move(body)) {}

IR_Node::IR_Node(std::unique_ptr<IR_Expr> body) : res(), body(std::move(body)) {}

IR_Node IR_Node::copy() const {
    if (res.has_value()) {
        if (body)
            return IR_Node(res->copy(), body->copy());
        else
            return IR_Node(res->copy(), nullptr);
    }
    else {
        if (body)
            return IR_Node(body->copy());
        else
            return IR_Node(nullptr);
    }
}


// Blocks

IR_Block::IR_Block(int id) : id(id) {}

void IR_Block::addNode(IR_Node node) {
    body.push_back(std::move(node));
}

void IR_Block::addNode(std::optional<IRval> res, std::unique_ptr<IR_Expr> expr) {
    if (res.has_value())
        body.emplace_back(res.value(), std::move(expr));
    else
        body.emplace_back(std::move(expr));
}

void IR_Block::addNewPhiNode(IRval res) {
    phis.emplace_back(std::move(res), std::make_unique<IR_ExprPhi>());
}

void IR_Block::setTerminator(IR_ExprTerminator::TermType type) {
    if (type == IR_ExprTerminator::BRANCH)
        semanticError("Branck term statement needs argument");
    termNode = IR_Node(std::make_unique<IR_ExprTerminator>(type));
}

void IR_Block::setTerminator(IR_ExprTerminator::TermType type, IRval arg) {
    if (type == IR_ExprTerminator::JUMP)
        semanticError("Jump term statement doesn't have arguments");
    termNode = IR_Node(std::make_unique<IR_ExprTerminator>(type, std::move(arg)));
}

IR_Block IR_Block::copy() const {
    IR_Block newBlock(id);
    for (auto const &phiNode : phis)
        newBlock.phis.push_back(phiNode.copy());
    for (auto const &node : body)
        newBlock.body.push_back(node.copy());
    newBlock.prev = prev;
    newBlock.next = next;
    if (termNode)
        newBlock.termNode = termNode->copy();
    else
        newBlock.termNode = {};
    return newBlock;
}

std::vector<IRval> IR_Block::getDefinitions() const {
    std::vector<IRval> defs;
    for (auto const &phiNode : phis)
        if (phiNode.res && phiNode.res->isVReg())
            defs.push_back(*phiNode.res);
    for (auto const &node : body)
        if (node.res && node.res->isVReg())
            defs.push_back(*node.res);
    return defs;
}

std::vector<IRval> IR_Block::getReferences() const {
    std::vector<IRval> refs;
    for (auto const &phiNode : phis)
        if (phiNode.body)
            for (IRval *arg : phiNode.body->getArgs())
                if (arg->isVReg())
                    refs.push_back(*arg);
    for (auto const &node : body)
        if (node.body)
            for (IRval *arg : node.body->getArgs())
                if (arg->isVReg())
                    refs.push_back(*arg);
    if (termNode)
        for (IRval *arg : termNode->body->getArgs())
            if (arg->isVReg())
                refs.push_back(*arg);
    return refs;
}

std::vector<IR_Node*> IR_Block::getAllNodes() {
    std::vector<IR_Node*> res;
    for (auto &phiNode : phis)
        res.push_back(&phiNode);
    for (auto &node : body)
        res.push_back(&node);
    if (termNode.has_value())
        res.push_back(&*termNode);
    return res;
}

IR_ExprTerminator const* IR_Block::getTerminator() const {
    if (!termNode)
        return nullptr;
    return dynamic_cast<IR_ExprTerminator *>(termNode->body.get());
}
