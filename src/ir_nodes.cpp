#include "ir_nodes.hpp"
#include "utils.hpp"

// IR_Type

IR_Type::IR_Type(IR_Type::Type type) : type(type) {}


// IR_TypeDirect

IR_TypeDirect::IR_TypeDirect(DirType spec) : IR_Type(IR_Type::DIRECT), spec(spec) {}

bool IR_TypeDirect::isInteger() const {
    return isInList(spec, { I8, U8, I32, U32, I64, U64 });
}

bool IR_TypeDirect::isFloat() const {
    return isInList(spec, { F32 });
}

bool IR_TypeDirect::isSigned() const {
    return isInList(spec, { I8, I32, I64 });
}

bool IR_TypeDirect::isUnsigned() const {
    return isInList(spec, { U8, U32, U64 });
}

int IR_TypeDirect::getBytesSize() const {
    switch (spec) {
        case VOID:
            return 0;
        case U8:
        case I8:
            return 1;
        case U32:
        case I32:
        case F32:
            return 4;
        case U64:
        case I64:
            return 8;
    }
    throw;
}

bool IR_TypeDirect::equal(const IR_Type &rhs) const {
    if (rhs.type != IR_Type::DIRECT)
        return false;
    auto const &rtype = dynamic_cast<IR_TypeDirect const &>(rhs);
    return spec == rtype.spec;
}


// IR_TypePtr

IR_TypePtr::IR_TypePtr(std::shared_ptr<IR_Type> child) :
        IR_Type(IR_Type::POINTER), child(child) {}

bool IR_TypePtr::equal(const IR_Type &rhs) const {
    if (rhs.type != IR_Type::POINTER)
        return false;
    auto const &rtype = dynamic_cast<IR_TypePtr const &>(rhs);
    return is_const == rtype.is_const && is_restrict == rtype.is_restrict &&
           is_volatile == rtype.is_volatile && this->child->equal(*rtype.child);
}

// IR_TypeFunc

IR_TypeFunc::IR_TypeFunc(std::shared_ptr<IR_Type> ret) :
        IR_Type(FUNCTION), ret(ret), args(), isVariadic(false) {}

IR_TypeFunc::IR_TypeFunc(std::shared_ptr<IR_Type> ret, std::vector<std::shared_ptr<IR_Type>> args, bool variadic) :
        IR_Type(IR_Type::FUNCTION), ret(ret), args(args), isVariadic(variadic) {}

bool IR_TypeFunc::equal(const IR_Type &rhs) const {
    if (rhs.type != IR_Type::FUNCTION)
        return false;
    auto const &rtype = dynamic_cast<IR_TypeFunc const &>(rhs);
    if (isVariadic != rtype.isVariadic || !ret->equal(*rtype.ret) || args.size() != rtype.args.size())
        return false;
    for (size_t i = 0; i < args.size(); i++) {
        if (!args[i]->equal(*rtype.args[i]))
            return false;
    }
    return true;
}


// IR_TypeArray

IR_TypeArray::IR_TypeArray(std::shared_ptr<IR_Type> child, uint64_t size) :
        IR_Type(IR_Type::ARRAY), child(child), size(size) {}

bool IR_TypeArray::equal(const IR_Type &rhs) const {
    if (rhs.type != IR_Type::ARRAY)
        return false;
    auto const &rtype = dynamic_cast<IR_TypeArray const &>(rhs);
    return size == rtype.size && child->equal(*rtype.child);
}


// Expressions

IR_Expr::IR_Expr(IR_Expr::Type type) : type(type) {}

IR_ExprOper::IR_ExprOper(IR_Ops op, std::vector<IRval> args) :
        IR_Expr(IR_Expr::OPERATION), op(op), args(args) {
    // TODO: check args count
}

std::string IR_ExprOper::opToString() const {
    switch (op) {
        case IR_MUL:    return "mul";
        case IR_DIV:    return "div";
        case IR_REM:    return "rem";
        case IR_ADD:    return "add";
        case IR_SUB:    return "sub";
        case IR_SHR:    return "shr";
        case IR_SHL:    return "shl";
        case IR_XOR:    return "xor";
        case IR_AND:    return "and";
        case IR_OR:     return "or";
        case IR_LAND:   return "land";
        case IR_LOR:    return "lor";
        case IR_EQ:     return "eq";
        case IR_NE:     return "ne";
        case IR_GT:     return "gt";
        case IR_LT:     return "lt";
        case IR_GE:     return "ge";
        case IR_LE:     return "le";
        case IR_DEREF:  return "deref";
        case IR_STORE:  return "store";
    }
    throw;
}


// IR_ExprAlloc

IR_ExprAlloc::IR_ExprAlloc(std::shared_ptr<IR_Type> type, size_t size) :
        IR_Expr(ALLOCATION), type(type), size(size) {}

IR_ExprAlloc::IR_ExprAlloc(std::shared_ptr<IR_Type> type, size_t size, bool onHeap) :
        IR_Expr(ALLOCATION), type(type), size(size), isOnHeap(onHeap) {}

std::string IR_ExprAlloc::opToString() const {
    return isOnHeap ? "malloc" : "alloca";
}


// Blocks

IR_Block::IR_Block(int id) : id(id) {}

void IR_Block::addNode(IR_Node node) {
    body.push_back(std::move(node));
}


// IRval

uint64_t IRval::regs_counter = 1;

IRval::IRval(std::shared_ptr<IR_Type> type, bool isReg, IRval::union_type v) :
        type(type), isReg(isReg), val(v) {}

std::shared_ptr<IR_Type> const& IRval::getType() const {
    return type;
}

IRval IRval::createVal(std::shared_ptr<IR_Type> type, IRval::union_type v) {
    return IRval(type, false, v);
}

IRval IRval::createReg(std::shared_ptr<IR_Type> type) {
    return IRval(type, true, regs_counter++);
}

bool IRval::isConstant() const {
    return !isReg;
}

const IRval::union_type &IRval::getVal() const {
    return val;
}

bool IRval::isRegister() const {
    return isReg;
}

std::string IRval::to_string() const {
    if (isReg)
        return std::visit([](auto e) -> std::string { return "%" + std::to_string(e); }, val);
    else
        return std::visit([](auto e) -> std::string { return std::to_string(e); }, val);
}


// IR_Node

IR_Node::IR_Node(IRval res, std::unique_ptr<IR_Expr> body) : res(res), body(std::move(body)) {}

IR_Node::IR_Node(std::unique_ptr<IR_Expr> body) : res(), body(std::move(body)) {}


// IR_Terminator

IR_Terminator::IR_Terminator() : type(NONE), val() {}

IR_Terminator::IR_Terminator(IR_Terminator::TermType type) : type(type), val() {}

IR_Terminator::IR_Terminator(IR_Terminator::TermType type, IRval val) : type(type), val(std::move(val)) {}
