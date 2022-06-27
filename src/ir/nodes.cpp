#include "nodes.hpp"
#include "utils.hpp"
#include <sstream>

// Expressions

IR_Expr::IR_Expr(std::optional<IRval> res) : res(std::move(res)) {}

std::unique_ptr<IR_Expr> IR_Expr::copy() const {
    auto newExpr = copyBody();
    if (res)
        newExpr->res = res->copy();
    else
        newExpr->res = std::nullopt;
    return newExpr;
}


// IR_ExprOper

IR_ExprOper::IR_ExprOper(std::optional<IRval> res, IR_ExprOper::IR_Ops op, std::vector<IRval> args)
        : IR_Expr(std::move(res)), op(op), args(std::move(args)) {} // TODO: check args count

std::unique_ptr<IR_ExprOper> IR_ExprOper::copyTyped() const {
    std::vector<IRval> newArgs;
    for (auto const &arg : args)
        newArgs.push_back(arg.copy());
    return std::make_unique<IR_ExprOper>(std::nullopt, op, std::move(newArgs));
}

std::unique_ptr<IR_Expr> IR_ExprOper::copyBody() const {
    return copyTyped();
}

std::vector<IRval*> IR_ExprOper::getArgs() {
    std::vector<IRval*> resArgs(args.size());
    for (size_t i = 0; auto &arg : args)
        resArgs.at(i++) = &arg;
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
        case IR_ExprOper::MOV:        return "mov";
        case IR_ExprOper::INTR_CTZ:         return "ctz";
        case IR_ExprOper::INTR_CLZ:         return "clz";
        case IR_ExprOper::INTR_POPCNT:      return "popcnt";
        case IR_ExprOper::INTR_BITREV:      return "bitrev";
    }
    throw std::logic_error("Wrong IR operation");
}

std::string IR_ExprOper::to_string() const {
    std::stringstream ss;
    ss << operOpToStr(op) << " ";
    for (const auto &arg : args)
        ss << arg.to_string() << " ";
    return ss.str();
}


// IR_ExprMem

IR_ExprMem::IR_ExprMem(std::optional<IRval> res, IR_ExprMem::MemOps op, IRval ptr)
        : IR_Expr(std::move(res)), op(op), addr(std::move(ptr)), val() {}

IR_ExprMem::IR_ExprMem(std::optional<IRval> res, IR_ExprMem::MemOps op, IRval ptr, IRval val)
        : IR_Expr(std::move(res)), op(op), addr(std::move(ptr)), val(std::move(val)) {}

std::unique_ptr<IR_ExprMem> IR_ExprMem::copyTyped() const {
    if (op == LOAD)
        return std::make_unique<IR_ExprMem>(std::nullopt, op, addr.copy());
    else if (op == STORE)
        return std::make_unique<IR_ExprMem>(std::nullopt, op, addr.copy(), val->copy());
    else
        throw std::logic_error("Wrong memory operation");
}

std::unique_ptr<IR_Expr> IR_ExprMem::copyBody() const {
    return copyTyped();
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
    throw std::logic_error("Wrong memory operation");
}


// IR_ExprAccess

// TODO: check op
IR_ExprAccess::IR_ExprAccess(std::optional<IRval> res, IR_ExprAccess::AccessOps op,
                             IRval base, std::vector<IRval> ind)
        : IR_Expr(std::move(res)), op(op), base(std::move(base)), indices(std::move(ind)) {}

IR_ExprAccess::IR_ExprAccess(std::optional<IRval> res, IR_ExprAccess::AccessOps op,
                             IRval base, IRval val, std::vector<IRval> ind)
        : IR_Expr(std::move(res)), op(op), base(std::move(base)), indices(std::move(ind)), val(std::move(val)) {}

std::unique_ptr<IR_ExprAccess> IR_ExprAccess::copyTyped() const {
    std::vector<IRval> newIndices;
    for (auto const &ind : indices)
        newIndices.push_back(ind.copy());
    if (val.has_value())
        return std::make_unique<IR_ExprAccess>(std::nullopt, op, base.copy(), val->copy(), std::move(newIndices));
    else
        return std::make_unique<IR_ExprAccess>(std::nullopt, op, base.copy(), std::move(newIndices));
}

std::unique_ptr<IR_Expr> IR_ExprAccess::copyBody() const {
    return copyTyped();
}

std::vector<IRval *> IR_ExprAccess::getArgs() {
    std::vector<IRval *> res;
    res.push_back(&base);
    if (val.has_value())
        res.push_back(&val.value());
    for (auto &ind : indices)
        res.push_back(&ind);
    return res;
}

std::string IR_ExprAccess::to_string() const {
    std::stringstream ss;
    switch (op) {
        case EXTRACT:
            ss << "extract ";
            break;
        case INSERT:
            ss << "insert ";
            break;
        case GEP:
            ss << "gep ";
            break;
        default:
            throw std::logic_error("Wrong access operation");
    }
    ss << base.to_string() << " ";
    if (val.has_value())
        ss << val->to_string() << " ";
    for (auto const &ind : indices)
        ss << ind.to_string() << " ";
    return ss.str();
}


// IR_ExprAlloc

IR_ExprAlloc::IR_ExprAlloc(std::optional<IRval> res, std::shared_ptr<IR_Type> type, size_t size)
        : IR_Expr(std::move(res)), type(std::move(type)), size(size) {}

IR_ExprAlloc::IR_ExprAlloc(std::optional<IRval> res, std::shared_ptr<IR_Type> type, size_t size, bool onHeap)
        : IR_Expr(std::move(res)), type(std::move(type)), size(size), isOnHeap(onHeap) {}

std::unique_ptr<IR_ExprAlloc> IR_ExprAlloc::copyTyped() const {
    return std::make_unique<IR_ExprAlloc>(std::nullopt, type->copy(), size, isOnHeap);
}

std::unique_ptr<IR_Expr> IR_ExprAlloc::copyBody() const {
    return copyTyped();
}

std::string IR_ExprAlloc::to_string() const {
    std::string opStr = isOnHeap ? "malloc" : "alloca";
    return fmt::format("{} {} x {}", opStr, type->to_string(), size);
}

std::vector<IRval*> IR_ExprAlloc::getArgs() {
    return {};
}


// IR_ExprCast

IR_ExprCast::IR_ExprCast(std::optional<IRval> res, IRval sourceVal, std::shared_ptr<IR_Type> cdest)
        : IR_Expr(std::move(res)), arg(std::move(sourceVal)), dest(std::move(cdest)) {
    const auto &source = arg.getType();
    if (source->equal(*dest))
        throw cw39_internal_error("Casting equal types");

    if (source->type == IR_Type::FUNCTION || dest->type == IR_Type::FUNCTION) {
        throw cw39_not_implemented("Function type cannot be cast"); // TODO
    }
    else if (source->type == IR_Type::ARRAY) {
        if (dest->type != IR_Type::POINTER)
            throw cw39_error("Array type can be cast only to pointer");
        castOp = BITCAST;
    }
    else if (source->type == IR_Type::POINTER && dest->type == IR_Type::POINTER) {
        castOp = BITCAST;
    }
    else if (source->type == IR_Type::POINTER && dest->type == IR_Type::DIRECT) {
        auto const &dstDir = dynamic_cast<IR_TypeDirect const &>(*dest);
        if (!dstDir.isInteger() || dstDir.spec != IR_TypeDirect::U64)
            throw cw39_error("Pointer can be cast only to u64 number");
        castOp = PTRTOI;
    }
    else if (source->type == IR_Type::DIRECT && dest->type == IR_Type::POINTER) {
        auto const &srcDir = dynamic_cast<IR_TypeDirect const &>(*source);
        if (!srcDir.isInteger() || srcDir.spec != IR_TypeDirect::U64)
            throw cw39_error("Pointer can be created only from u64 number");
        castOp = ITOPTR;
    }
    else if (source->type == IR_Type::DIRECT && dest->type == IR_Type::DIRECT) {
        auto const &srcDir = dynamic_cast<IR_TypeDirect const &>(*source);
        auto const &dstDir = dynamic_cast<IR_TypeDirect const &>(*dest);

        if (dstDir.spec == IR_TypeDirect::VOID)
            throw cw39_error("Casts to void are not allowed");

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
                throw cw39_internal_error("Wrong float conversion");
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

std::unique_ptr<IR_ExprCast> IR_ExprCast::copyTyped() const {
    return std::make_unique<IR_ExprCast>(std::nullopt, arg.copy(), dest->copy());
}

std::unique_ptr<IR_Expr> IR_ExprCast::copyBody() const {
    return copyTyped();
}

std::vector<IRval*> IR_ExprCast::getArgs() {
    return std::vector<IRval*>{ &arg };
}

static std::string castOpToStr(IR_ExprCast::CastType op)  {
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
    throw std::logic_error("Wrong cast operation type");
}

std::string IR_ExprCast::to_string() const {
    return fmt::format("{} {} : {}", castOpToStr(castOp), arg.to_string(), dest->to_string());
}


// IR_ExprCall

IR_ExprCall::IR_ExprCall(std::optional<IRval> res, int callee, std::vector<IRval> args)
        : IR_Expr(std::move(res)), callee(callee), args(std::move(args)) {}

IR_ExprCall::IR_ExprCall(std::optional<IRval> res, IRval callee, std::vector<IRval> args)
        : IR_Expr(std::move(res)), callee(callee), args(std::move(args)) {}

std::unique_ptr<IR_ExprCall> IR_ExprCall::copyTyped() const {
    std::vector<IRval> newArgs;
    for (auto const &arg : args)
        newArgs.push_back(arg.copy());
    if (isIndirect())
        return std::make_unique<IR_ExprCall>(std::nullopt, getFuncPtr().copy(), std::move(newArgs));
    else
        return std::make_unique<IR_ExprCall>(std::nullopt, getFuncId(), std::move(newArgs));
}

std::unique_ptr<IR_Expr> IR_ExprCall::copyBody() const {
    return copyTyped();
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

IR_ExprPhi::IR_ExprPhi(std::optional<IRval> res) : IR_Expr(std::move(res)) {}

std::unique_ptr<IR_ExprPhi> IR_ExprPhi::copyTyped() const {
    auto res = std::make_unique<IR_ExprPhi>(std::nullopt);
    for (auto const &[pos, arg] : args)
        res->args.emplace(pos, arg.copy());
    return res;
}

std::unique_ptr<IR_Expr> IR_ExprPhi::copyBody() const {
    return copyTyped();
}

std::vector<IRval *> IR_ExprPhi::getArgs() {
    std::vector<IRval*> resArgs(args.size());
    for (size_t i = 0; auto &[pos, arg] : args)
        resArgs.at(i++) = &arg;
    return resArgs;
}


// IR_ExprTerminator

IR_ExprTerminator::IR_ExprTerminator(IR_ExprTerminator::TermType type)
        : IR_Expr(std::nullopt), termType(type), arg() {}

IR_ExprTerminator::IR_ExprTerminator(IR_ExprTerminator::TermType type, IRval val)
        : IR_Expr(std::nullopt), termType(type), arg(std::move(val)) {}

std::unique_ptr<IR_ExprTerminator> IR_ExprTerminator::copyTyped() const {
    if (arg.has_value())
        return std::make_unique<IR_ExprTerminator>(termType, arg->copy());
    else
        return std::make_unique<IR_ExprTerminator>(termType);
}

std::unique_ptr<IR_Expr> IR_ExprTerminator::copyBody() const {
    return copyTyped();
}

std::vector<IRval *> IR_ExprTerminator::getArgs() {
    if (arg.has_value())
        return std::vector<IRval *>{ &(*arg) };
    else
        return std::vector<IRval *>();
}


// IR_ExprNOP

IR_ExprNOP::IR_ExprNOP() : IR_Expr(std::nullopt) {}

std::vector<IRval *> IR_ExprNOP::getArgs() {
    return {};
}

std::unique_ptr<IR_Expr> IR_ExprNOP::copyBody() const {
    return std::make_unique<IR_ExprNOP>();
}


// Blocks

IR_Block::IR_Block(int id) : id(id) {}

void IR_Block::addNode(std::unique_ptr<IR_Expr> expr) {
    body.emplace_back(std::move(expr));
}

IR_ExprPhi& IR_Block::addNewPhiNode(IRval res) {
    auto &it = phis.emplace_back(std::make_unique<IR_ExprPhi>(std::move(res)));
    return static_cast<IR_ExprPhi&>(*it);
}

void IR_Block::setTerminator(IR_ExprTerminator::TermType type) {
    if (type == IR_ExprTerminator::BRANCH)
        throw cw39_internal_error("Branck term statement needs argument");
    termNode = std::make_unique<IR_ExprTerminator>(type);
}

void IR_Block::setTerminator(IR_ExprTerminator::TermType type, IRval arg) {
    if (type == IR_ExprTerminator::JUMP)
        throw cw39_internal_error("Jump term statement doesn't have arguments");
    termNode = std::make_unique<IR_ExprTerminator>(type, std::move(arg));
}

IR_Block IR_Block::copy() const {
    IR_Block newBlock(id);
    for (auto const &phiNode : phis)
        newBlock.phis.push_back(phiNode->copy());
    for (auto const &node : body)
        newBlock.body.push_back(node->copy());
    newBlock.prev = prev;
    newBlock.next = next;
    if (termNode)
        newBlock.termNode = termNode->copyTyped();
    else
        newBlock.termNode = {};
    return newBlock;
}

std::vector<IRval> IR_Block::getDefinitions() const {
    std::vector<IRval> defs;
    for (auto const &phiNode : phis)
        if (phiNode->res && phiNode->res->isVReg())
            defs.push_back(*phiNode->res);
    for (auto const &node : body)
        if (node->res && node->res->isVReg())
            defs.push_back(*node->res);
    return defs;
}

std::vector<IRval> IR_Block::getReferences() const {
    std::vector<IRval> refs;
    for (auto const &phiNode : phis) {
        if (phiNode)
            for (IRval *arg : phiNode->getArgs())
                if (arg->isVReg())
                    refs.push_back(*arg);
    }
    for (auto const &node : body) {
        if (node)
            for (IRval *arg : node->getArgs())
                if (arg->isVReg())
                    refs.push_back(*arg);
    }
    if (termNode) {
        for (IRval *arg : termNode->getArgs())
            if (arg->isVReg())
                refs.push_back(*arg);
    }
    return refs;
}

std::vector<IR_Expr*> IR_Block::getAllNodes() {
    std::vector<IR_Expr*> res;
    for (auto &phiNode : phis)
        res.push_back(phiNode.get());
    for (auto &node : body)
        res.push_back(node.get());
    if (termNode)
        res.push_back(termNode.get());
    return res;
}

IR_ExprTerminator const* IR_Block::getTerminator() const {
    return termNode.get();
}

void IR_Block::removePredecessor(int pred) {
    std::vector<int> newPrev;
    for (int blockId : prev) {
        if (blockId != pred)
            newPrev.push_back(blockId);
    }
    prev = std::move(newPrev);
}
