#include "types.hpp"
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
    return isInList(spec, { F32, F64 });
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
        case F64:
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


std::shared_ptr<IR_TypeDirect> IR_TypeDirect::staticTypes[9] = {};

std::shared_ptr<IR_TypeDirect> IR_TypeDirect::getStaticType(size_t index, DirType type) {
    if (!staticTypes[index])
        staticTypes[index] = std::make_shared<IR_TypeDirect>(type);
    return staticTypes[index];
}

std::shared_ptr<IR_TypeDirect> IR_TypeDirect::getVoid() {
    return getStaticType(0, IR_TypeDirect::VOID);
}

std::shared_ptr<IR_TypeDirect> IR_TypeDirect::getI8() {
    return getStaticType(1, IR_TypeDirect::I8);
}

std::shared_ptr<IR_TypeDirect> IR_TypeDirect::getU8() {
    return getStaticType(2, IR_TypeDirect::U8);
}

std::shared_ptr<IR_TypeDirect> IR_TypeDirect::getI32() {
    return getStaticType(3, IR_TypeDirect::I32);
}

std::shared_ptr<IR_TypeDirect> IR_TypeDirect::getU32() {
    return getStaticType(4, IR_TypeDirect::U32);
}

std::shared_ptr<IR_TypeDirect> IR_TypeDirect::getI64() {
    return getStaticType(5, IR_TypeDirect::I64);
}

std::shared_ptr<IR_TypeDirect> IR_TypeDirect::getU64() {
    return getStaticType(6, IR_TypeDirect::U64);
}

std::shared_ptr<IR_TypeDirect> IR_TypeDirect::getF32() {
    return getStaticType(7, IR_TypeDirect::F32);
}

std::shared_ptr<IR_TypeDirect> IR_TypeDirect::getF64() {
    return getStaticType(8, IR_TypeDirect::F64);
}


// IR_TypeStruct

IR_TypeStruct::IR_TypeStruct(string_id_t ident, std::vector<StructField> fields)
        : IR_Type(TSTRUCT), structId(ident), fields(std::move(fields)) {}

IR_TypeStruct::StructField::StructField(string_id_t ident, std::shared_ptr<IR_Type> type, int index)
        : fieldName(ident), irType(std::move(type)), index(index) {}

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
    int res = 0;
    for (const auto &field : fields) {
        int cur = field.irType->getBytesSize();
        res += (res % cur) + cur;
    }
    res += res % 8;
    return res;
}

IR_TypeStruct::StructField const* IR_TypeStruct::getField(string_id_t id) const {
    for (auto const &field : fields)
        if (field.fieldName == id)
            return &field;
    return nullptr;
}


// IR_TypePtr

IR_TypePtr::IR_TypePtr(std::shared_ptr<IR_Type> child) :
        IR_Type(IR_Type::POINTER), child(std::move(child)) {}

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
        IR_Type(FUNCTION), ret(std::move(ret)), args(), isVariadic(false) {}

IR_TypeFunc::IR_TypeFunc(std::shared_ptr<IR_Type> ret, std::vector<std::shared_ptr<IR_Type>> args, bool variadic) :
        IR_Type(IR_Type::FUNCTION), ret(std::move(ret)), args(std::move(args)), isVariadic(variadic) {}

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
    semanticError("Cannot get size of function type");
}


// IR_TypeArray

IR_TypeArray::IR_TypeArray(std::shared_ptr<IR_Type> child, uint64_t size) :
        IR_Type(IR_Type::ARRAY), child(std::move(child)), size(size) {}

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
    return static_cast<int>(size) * child->getBytesSize();
}
