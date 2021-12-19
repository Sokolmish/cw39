#include "nodes.hpp"
#include "utils.hpp"

// IR_StorageSpecifier

IR_StorageSpecifier::IR_StorageSpecifier() : spec(AUTO) {}

IR_StorageSpecifier::IR_StorageSpecifier(IR_StorageSpecifier::Specs spec) : spec(spec) {}


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

std::shared_ptr<IR_Type> IR_TypeDirect::copy() const {
    return std::make_shared<IR_TypeDirect>(spec);
}

std::shared_ptr<IR_TypeDirect> IR_TypeDirect::type_void =
        std::make_shared<IR_TypeDirect>(IR_TypeDirect::VOID);
std::shared_ptr<IR_TypeDirect> IR_TypeDirect::type_i8 =
        std::make_shared<IR_TypeDirect>(IR_TypeDirect::I8);
std::shared_ptr<IR_TypeDirect> IR_TypeDirect::type_u8 =
        std::make_shared<IR_TypeDirect>(IR_TypeDirect::U8);
std::shared_ptr<IR_TypeDirect> IR_TypeDirect::type_i32 =
        std::make_shared<IR_TypeDirect>(IR_TypeDirect::I32);
std::shared_ptr<IR_TypeDirect> IR_TypeDirect::type_u32 =
        std::make_shared<IR_TypeDirect>(IR_TypeDirect::U32);
std::shared_ptr<IR_TypeDirect> IR_TypeDirect::type_i64 =
        std::make_shared<IR_TypeDirect>(IR_TypeDirect::I64);
std::shared_ptr<IR_TypeDirect> IR_TypeDirect::type_u64 =
        std::make_shared<IR_TypeDirect>(IR_TypeDirect::U64);
std::shared_ptr<IR_TypeDirect> IR_TypeDirect::type_f32 =
        std::make_shared<IR_TypeDirect>(IR_TypeDirect::F32);


// IR_TypeStruct

IR_TypeStruct::IR_TypeStruct(string_id_t ident, std::vector<StructField> fields)
        : IR_Type(TSTRUCT), structId(ident), fields(std::move(fields)) {}

IR_TypeStruct::StructField::StructField(string_id_t ident, std::shared_ptr<IR_Type> type, int index)
        : fieldName(ident), irType(type), index(index) {}

bool IR_TypeStruct::equal(IR_Type const &rhs) const {
    if (rhs.type != IR_Type::TSTRUCT)
        return false;
    auto const &rtype = dynamic_cast<IR_TypeStruct const &>(rhs);
    return structId == rtype.structId;
}

std::shared_ptr<IR_Type> IR_TypeStruct::copy() const {
    return std::make_shared<IR_TypeStruct>(structId, fields);
}

int IR_TypeStruct::getBytesSize() const {
    NOT_IMPLEMENTED("Structs size");
}

IR_TypeStruct::StructField const* IR_TypeStruct::getField(string_id_t id) const {
    for (auto const &field : fields)
        if (field.fieldName == id)
            return &field;
    return nullptr;
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

std::shared_ptr<IR_Type> IR_TypePtr::copy() const {
    auto res = std::make_shared<IR_TypePtr>(child->copy());
    res->is_const = is_const;
    res->is_volatile = is_volatile;
    res->is_restrict = is_restrict;
    return res;
}

int IR_TypePtr::getBytesSize() const {
    return 8;
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

std::shared_ptr<IR_Type> IR_TypeFunc::copy() const {
    auto retCopy = ret->copy();
    std::vector<std::shared_ptr<IR_Type>> argsCopy;
    for (auto &arg : args)
        argsCopy.push_back(arg->copy());
    return std::make_shared<IR_TypeFunc>(retCopy, argsCopy, isVariadic);
}

int IR_TypeFunc::getBytesSize() const {
    throw std::runtime_error("Cannot get size of function type");
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

std::shared_ptr<IR_Type> IR_TypeArray::copy() const {
    return std::make_shared<IR_TypeArray>(child->copy(), size);
}

int IR_TypeArray::getBytesSize() const {
    return size * child->getBytesSize();
}


// Expressions

IR_Expr::IR_Expr(IR_Expr::Type type) : type(type) {}


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
        case IR_MUL:        return "mul";
        case IR_DIV:        return "div";
        case IR_REM:        return "rem";
        case IR_ADD:        return "add";
        case IR_SUB:        return "sub";
        case IR_SHR:        return "shr";
        case IR_SHL:        return "shl";
        case IR_XOR:        return "xor";
        case IR_AND:        return "and";
        case IR_OR:         return "or";
        case IR_LAND:       return "land";
        case IR_LOR:        return "lor";
        case IR_EQ:         return "eq";
        case IR_NE:         return "ne";
        case IR_GT:         return "gt";
        case IR_LT:         return "lt";
        case IR_GE:         return "ge";
        case IR_LE:         return "le";
        case IR_LOAD:       return "load";
        case IR_STORE:      return "store";
        case IR_EXTRACT:    return "extract";
        case IR_INSERT:     return "insert";
        case IR_MOV:        return "mov";
//        case IR_GEP:        return "gep";
    }
    throw;
}


// IR_ExprAlloc

IR_ExprAlloc::IR_ExprAlloc(std::shared_ptr<IR_Type> type, size_t size) :
        IR_Expr(ALLOCATION), type(type), size(size) {}

IR_ExprAlloc::IR_ExprAlloc(std::shared_ptr<IR_Type> type, size_t size, bool onHeap) :
        IR_Expr(ALLOCATION), type(type), size(size), isOnHeap(onHeap) {}

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

IR_ExprCast::IR_ExprCast(IRval sourceVal, std::shared_ptr<IR_Type> dest)
        : IR_Expr(CAST), arg(sourceVal), dest(dest) {
    auto source = sourceVal.getType();
    if (source->equal(*dest))
        semanticError("Casting equal types");

    if (source->type == IR_Type::FUNCTION || dest->type == IR_Type::FUNCTION) {
        semanticError("Function type cannot be cast");
    }
    else if (source->type == IR_Type::ARRAY || dest->type == IR_Type::ARRAY) {
        semanticError("Array type cannot be cast");
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

        if (srcDir.isInteger() && dstDir.isInteger()) {
            if (srcDir.getBytesSize() == dstDir.getBytesSize())
                castOp = BITCAST;
            else if (srcDir.getBytesSize() > dstDir.getBytesSize())
                castOp = TRUNC;
            else if (srcDir.isSigned() && dstDir.isSigned())
                castOp = SEXT;
            else
                castOp = ZEXT; // TODO: maybe wrong conversion here
        }
        else if (srcDir.isFloat() && dstDir.isFloat()) {
            NOT_IMPLEMENTED("Float-float conversion");
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
    }
    throw;
}


// IR_ExprCall

IR_ExprCall::IR_ExprCall(int callee, std::vector<IRval> args)
        : IR_Expr(CALL), funcId(callee), args(std::move(args)) {}

std::unique_ptr<IR_Expr> IR_ExprCall::copy() const {
    std::vector<IRval> newArgs;
    for (auto const &arg : args)
        newArgs.push_back(arg.copy());
    return std::make_unique<IR_ExprCall>(funcId, std::move(newArgs));
}

std::vector<IRval*> IR_ExprCall::getArgs() {
    std::vector<IRval*> resArgs;
    for (auto &arg : args)
        resArgs.push_back(&arg);
    return resArgs;
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


// Blocks

IR_Block::IR_Block(int id) : id(id) {}

void IR_Block::addNode(IR_Node node) {
    body.push_back(std::move(node));
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
    for (auto const &node : body)
        if (node.res)
            defs.push_back(*node.res);
    return defs;
}

std::vector<IRval> IR_Block::getReferences() const {
    std::vector<IRval> refs;
    for (auto const &node : body)
        if (node.body)
            for (IRval *arg : node.body->getArgs())
                refs.push_back(*arg);
    return refs;
}

std::vector<IR_Node*> IR_Block::getAllNodes() {
    std::vector<IR_Node*> res;
    for (auto &phiNode : phis)
        res.push_back(&phiNode);
    for (auto &node : body)
        res.push_back(&node);
    return res;
}

IR_ExprTerminator const* IR_Block::getTerminator() const {
    if (!termNode)
        return nullptr;
    return dynamic_cast<IR_ExprTerminator *>(termNode->body.get());
}


// IRval

IRval::IRval(std::shared_ptr<IR_Type> type, ValueClass vclass, IRval::union_type v) :
        type(type), valClass(vclass), val(v) {}

std::shared_ptr<IR_Type> const& IRval::getType() const {
    return type;
}

IRval IRval::createVal(std::shared_ptr<IR_Type> type, IRval::union_type v) {
    return IRval(type, IRval::VAL, v);
}

IRval IRval::createReg(std::shared_ptr<IR_Type> type, uint64_t id) {
    return IRval(type, IRval::VREG, id);
}

IRval IRval::createFunArg(std::shared_ptr<IR_Type> type, uint64_t num) {
    return IRval(type, IRval::FUN_PARAM, num);
}

IRval IRval::createString(uint64_t num) {
    auto strType = std::make_shared<IR_TypePtr>(IR_TypeDirect::type_i8);
//    strType->is_const = true;
    return IRval(strType, IRval::STRING, num);
}

IRval IRval::createDefault(std::shared_ptr<IR_Type> type) {
    if (type->type != IR_Type::DIRECT)
        semanticError("Cannot create default value for not direct type");
    auto dirType = std::dynamic_pointer_cast<IR_TypeDirect>(type);

    switch (dirType->spec) {
        case IR_TypeDirect::I8:
            return IRval(type, IRval::VAL, static_cast<int8_t>(0));
        case IR_TypeDirect::U8:
            return IRval(type, IRval::VAL, static_cast<uint8_t>(0));
        case IR_TypeDirect::I32:
            return IRval(type, IRval::VAL, static_cast<int32_t>(0));
        case IR_TypeDirect::U32:
            return IRval(type, IRval::VAL, static_cast<uint32_t>(0));
        case IR_TypeDirect::I64:
            return IRval(type, IRval::VAL, static_cast<int64_t>(0));
        case IR_TypeDirect::U64:
            return IRval(type, IRval::VAL, static_cast<uint64_t>(0));
        case IR_TypeDirect::F32:
            return IRval(type, IRval::VAL, static_cast<float>(0));
        case IR_TypeDirect::VOID:
            semanticError("Cannot create value of type VOID");
    }
    throw;
}


bool IRval::isConstant() const {
    return valClass == IRval::VAL;
}

bool IRval::isRegister() const {
    return valClass == IRval::VREG;
}

bool IRval::isGlobal() const {
    return valClass == IRval::GLOBAL;
}

const IRval::union_type &IRval::getVal() const {
    return val;
}

std::string IRval::to_string() const {
    switch (valClass) {
        case IRval::VREG:
            return std::visit([this](auto e) -> std::string {
                if (version)
                    return fmt::format("%{}.{}", e, *version);
                else
                    return fmt::format("%{}", e);
            }, val);

        case IRval::VAL:
            return std::visit([](auto e) -> std::string {
                return fmt::format("{}", e);
            }, val);

        case IRval::FUN_PARAM:
            return std::visit([](auto e) -> std::string {
                return fmt::format("%%arg{}", e);
            }, val);

        case IRval::GLOBAL:
            return std::visit([](auto e) -> std::string {
                return fmt::format("@{}", e);
            }, val);

        case IRval::STRING:
            return std::visit([](auto e) -> std::string {
                return fmt::format("@str_{}", e);
            }, val);
    }
    throw;
}

IRval IRval::copy() const {
    auto res = IRval(type->copy(), valClass, val);
    res.version = version;
    return res;
}

bool IRval::operator==(const IRval &oth) const {
    return valClass == oth.valClass && type->equal(*oth.type) && val == oth.val;
}

bool IRval::less(const IRval &a, const IRval &b) {
    if (a.valClass < b.valClass)
        return true;
    else if (a.valClass > b.valClass)
        return false;
    else
        return a.val < b.val;
}

//bool IRval::lessVersions(const IRval &a, const IRval &b) {
//    if (a.valClass < b.valClass)
//        return true;
//    else if (a.valClass > b.valClass)
//        return false;
//    else {
//        if (a.val < b.val)
//            return true;
//        else if (a.val > b.val)
//            return false;
//        else if (a.version.has_value() && b.version.has_value())
//            return a.version < b.version;
//        else
//            return a.version < b.version;
//    }
//}

bool IRval::Comparator::operator()(const IRval &a, const IRval &b) const {
    return IRval::less(a, b);
}

//bool IRval::ComparatorVersions::operator()(const IRval &a, const IRval &b) const {
//    return IRval::lessVersions(a, b);
//}


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
