#include "ir_nodes.hpp"
#include "utils.hpp"

// Types

IR_Type::IR_Type(IR_Type::Type type) : type(type) {}

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

IR_TypePtr::IR_TypePtr(std::shared_ptr<IR_Type> child) :
        IR_Type(IR_Type::POINTER), child(child) {}

IR_TypeFunc::IR_TypeFunc(std::shared_ptr<IR_Type> ret) :
        IR_Type(FUNCTION), ret(ret), args(), isVariadic(false) {}

IR_TypeFunc::IR_TypeFunc(std::shared_ptr<IR_Type> ret, std::vector<std::shared_ptr<IR_Type>> args, bool variadic) :
        IR_Type(IR_Type::FUNCTION), ret(ret), args(args), isVariadic(variadic) {}

IR_TypeArray::IR_TypeArray(std::shared_ptr<IR_Type> child, uint64_t size) :
        IR_Type(IR_Type::ARRAY), child(child), size(size) {}


// Expressions

IR_Expr::IR_Expr(IR_Expr::Type type) : type(type) {}

IR_ExprOper::IR_ExprOper(IR_Ops op, std::vector<IRval> args) :
    IR_Expr(IR_Expr::OPERATION), op(op), args(args)
{
    // TODO: check args count
}

std::string IR_ExprOper::opToString() const {
    switch (op) {
        case IR_MUL:    return  "mul";
        case IR_DIV:    return  "div";
        case IR_REM:    return  "rem";
        case IR_ADD:    return  "add";
        case IR_SUB:    return  "sub";
        case IR_SHR:    return  "shr";
        case IR_SHL:    return  "shl";
        case IR_XOR:    return  "xor";
        case IR_AND:    return  "and";
        case IR_OR:     return  "or";
        case IR_LAND:   return  "land";
        case IR_LOR:    return  "lor";
        case IR_NOT:    return  "not";
        case IR_NEG:    return  "neg";
        case IR_DEREF:  return  "deref";
        case IR_ALLOCA: return "alloca";
    }
    throw;
}


// Blocks

IR_Block::IR_Block(int id) : id(id) {}

void IR_Block::addNext(IR_Block &block) {
    next.push_back(&block);
    block.prev.push_back(this);
}

void IR_Block::addPrev(IR_Block &block) {
    prev.push_back(&block);
    block.next.push_back(this);
}

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
