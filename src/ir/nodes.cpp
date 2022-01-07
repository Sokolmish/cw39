#include "nodes.hpp"
#include "utils.hpp"
#include <sstream>
#include <fmt/ostream.h>

// Expressions

IR_Expr::IR_Expr(IR_Expr::Type type) : type(type) {}

IR_ExprOper const &IR_Expr::getOper() const {
    return dynamic_cast<IR_ExprOper const&>(*this);
}

IR_ExprMem const &IR_Expr::getMem() const {
    return dynamic_cast<IR_ExprMem const&>(*this);
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

static std::string operOpToStr(IR_ExprOper::IR_Ops op) {
    switch (op) {
        case IR_ExprOper::MUL:        return "mul";
        case IR_ExprOper::DIV:        return "div";
        case IR_ExprOper::REM:        return "rem";
        case IR_ExprOper::ADD:        return "add";
        case IR_ExprOper::SUB:        return "sub";
        case IR_ExprOper::SHR:        return "shr";
        case IR_ExprOper::SHL:        return "shl";
        case IR_ExprOper::XOR:        return "xor";
        case IR_ExprOper::AND:        return "and";
        case IR_ExprOper::OR:         return "or";
        case IR_ExprOper::LAND:       return "land";
        case IR_ExprOper::LOR:        return "lor";
        case IR_ExprOper::EQ:         return "eq";
        case IR_ExprOper::NE:         return "ne";
        case IR_ExprOper::GT:         return "gt";
        case IR_ExprOper::LT:         return "lt";
        case IR_ExprOper::GE:         return "ge";
        case IR_ExprOper::LE:         return "le";
        case IR_ExprOper::EXTRACT:    return "extract";
        case IR_ExprOper::INSERT:     return "insert";
        case IR_ExprOper::MOV:        return "mov";
    }
    throw;
}

std::string IR_ExprOper::to_string() const {
    std::stringstream ss;
    ss << operOpToStr(op) << " ";
    for (const auto &arg : args)
        ss << arg.to_string() << " ";
    return ss.str();
}


// IR_ExprMem

IR_ExprMem::IR_ExprMem(IR_ExprMem::MemOps op, IRval ptr)
        : IR_Expr(MEMORY), op(op), addr(std::move(ptr)), val() {}

IR_ExprMem::IR_ExprMem(IR_ExprMem::MemOps op, IRval ptr, IRval val)
        : IR_Expr(MEMORY), op(op), addr(std::move(ptr)), val(std::move(val)) {}

std::unique_ptr<IR_Expr> IR_ExprMem::copy() const {
    if (op == LOAD)
        return std::make_unique<IR_ExprMem>(op, addr.copy());
    else if (op == STORE)
        return std::make_unique<IR_ExprMem>(op, addr.copy(), val->copy());
    else
        internalError("Wrong memory operation");
}

std::vector<IRval *> IR_ExprMem::getArgs() {
    if (val.has_value())
        return { &addr, &val.value() };
    else
        return { &addr };
}

std::string IR_ExprMem::to_string() const {
    switch (op) {
        case LOAD:
            return fmt::format("load {}", addr.to_string());
        case STORE:
            return fmt::format("store {} {}", addr.to_string(), val->to_string());
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

std::string IR_ExprAlloc::to_string() const {
    std::string opStr = isOnHeap ? "malloc" : "alloca";
    return fmt::format("{} {} x {}", opStr, type->to_string(), size);
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

std::string castOpToStr(IR_ExprCast::CastType op)  {
    switch (op) {
        case IR_ExprCast::BITCAST:   return "bitcast";
        case IR_ExprCast::SEXT:      return "sext";
        case IR_ExprCast::ZEXT:      return "zext";
        case IR_ExprCast::TRUNC:     return "trunc";
        case IR_ExprCast::FPTOUI:    return "fp_to_ui";
        case IR_ExprCast::FPTOSI:    return "fp_to_si";
        case IR_ExprCast::UITOFP:    return "ui_to_fp";
        case IR_ExprCast::SITOFP:    return "si_to_fp";
        case IR_ExprCast::PTRTOI:    return "ptrtoi";
        case IR_ExprCast::ITOPTR:    return "itoptr";
        case IR_ExprCast::FPEXT:     return "fpext";
        case IR_ExprCast::FPTRUNC:   return "fptrunc";
    }
    throw;
}

std::string IR_ExprCast::to_string() const {
    return fmt::format("{} {} : {}", castOpToStr(castOp), arg.to_string(), dest->to_string());
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
