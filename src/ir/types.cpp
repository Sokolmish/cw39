#include "types.hpp"
#include "utils.hpp"
#include <sstream>

// IR_StorageSpecifier

IR_StorageSpecifier::IR_StorageSpecifier() : spec(AUTO) {}

IR_StorageSpecifier::IR_StorageSpecifier(IR_StorageSpecifier::Specs spec) : spec(spec) {}


// IR_TypeDirect

bool IR_TypeDirect::isInteger() const {
    return isInList(spec, BOOL, I8, U8, I32, U32, I64, U64);
}

bool IR_TypeDirect::isFloat() const {
    return isInList(spec, F32, F64);
}

bool IR_TypeDirect::isSigned() const {
    return isInList(spec, I8, I32, I64);
}

bool IR_TypeDirect::isUnsigned() const {
    return isInList(spec, U8, U32, U64);
}

int IR_TypeDirect::getBytesSize() const {
    switch (spec) {
        case VOID:
            return 0;
        case BOOL:
            return 1; // Bug?
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
    throw std::logic_error("Wrong direct type");
}

bool IR_TypeDirect::equal(const IR_Type &rhs) const {
    if (auto rtype = rhs.castType<IR_TypeDirect>())
        return spec == rtype->spec;
    return false;
}

std::shared_ptr<IR_Type> IR_TypeDirect::copy() const {
    return std::make_shared<IR_TypeDirect>(spec);
}

std::shared_ptr<IR_Type> IR_TypeDirect::getCommonDirType(std::shared_ptr<IR_Type> const &a,
                                                         std::shared_ptr<IR_Type> const &b) {
    if (a->equal(*b))
        return a;

    auto &aDir = dynamic_cast<IR_TypeDirect const &>(*a);
    auto &bDir = dynamic_cast<IR_TypeDirect const &>(*b);

    auto newRawType = aDir.copy(); // TODO: qualifiers
    auto &newType = dynamic_cast<IR_TypeDirect &>(*newRawType);

    // Float conversions

    if (aDir.spec == F64 || bDir.spec == F64) {
        newType.spec = F64;
        return newRawType;
    }
    else if (aDir.spec == F32 || bDir.spec == F32) {
        newType.spec = F32;
        return newRawType;
    }

    // Integer conversions

    if (aDir.isUnsigned() == bDir.isUnsigned()) {
        int newSize = std::max(aDir.getBytesSize(), bDir.getBytesSize());
        bool isUnsigned = aDir.isUnsigned();
        switch (newSize) {
            case 1:
                newType.spec = isUnsigned ? U8 : I8;
                break;
            case 4:
                newType.spec = isUnsigned ? U32 : I32;
                break;
            case 8:
                newType.spec = isUnsigned ? U64 : I64;
                break;
            default:
                throw cw39_internal_error(fmt::format("Wrong type size: {}", newSize));
        }
        return newRawType;
    }
    else { // Different signedness
        if (aDir.getBytesSize() == bDir.getBytesSize()) {
            if (aDir.isUnsigned())
                return a->copy();
            else
                return b->copy();
        }

        if (aDir.getBytesSize() < bDir.getBytesSize())
            return getCommonDirType(b, a);
        // Now we have sizeof(aDir) > sizeof(bDIr)
        if (aDir.isUnsigned()) {
            return a->copy();
        }
        else { // aDir is signed
            switch (aDir.getBytesSize()) {
                case 1:
                    newType.spec = U8;
                    break;
                case 4:
                    newType.spec = U32;
                    break;
                case 8:
                    newType.spec = U64;
                    break;
                default:
                    throw cw39_internal_error(fmt::format("Wrong type size: {}", aDir.getBytesSize()));
            }
            return newRawType;
        }
    }
}

std::string IR_TypeDirect::to_string() const {
    switch (spec) {
        case IR_TypeDirect::BOOL:
            return "i1";
        case IR_TypeDirect::U8:
            return "u8";
        case IR_TypeDirect::I8:
            return "i8";
        case IR_TypeDirect::U32:
            return "u32";
        case IR_TypeDirect::I32:
            return "i32";
        case IR_TypeDirect::U64:
            return "u64";
        case IR_TypeDirect::I64:
            return "i64";
        case IR_TypeDirect::F32:
            return "f32";
        case IR_TypeDirect::F64:
            return "f64";
        case IR_TypeDirect::VOID:
            return "void";
    }
    throw std::logic_error("Wrong direct type");
}


std::shared_ptr<IR_TypeDirect> IR_TypeDirect::staticTypes[10] = {};

std::shared_ptr<IR_TypeDirect> IR_TypeDirect::getStaticType(size_t index, DirType type) {
    if (!staticTypes[index])
        staticTypes[index] = std::make_shared<IR_TypeDirect>(type);
    return staticTypes[index];
}

std::shared_ptr<IR_TypeDirect> IR_TypeDirect::getVoid() {
    return getStaticType(0, IR_TypeDirect::VOID);
}

std::shared_ptr<IR_TypeDirect> IR_TypeDirect::getI1() {
    return getStaticType(1, IR_TypeDirect::BOOL);
}

std::shared_ptr<IR_TypeDirect> IR_TypeDirect::getI8() {
    return getStaticType(2, IR_TypeDirect::I8);
}

std::shared_ptr<IR_TypeDirect> IR_TypeDirect::getU8() {
    return getStaticType(3, IR_TypeDirect::U8);
}

std::shared_ptr<IR_TypeDirect> IR_TypeDirect::getI32() {
    return getStaticType(4, IR_TypeDirect::I32);
}

std::shared_ptr<IR_TypeDirect> IR_TypeDirect::getU32() {
    return getStaticType(5, IR_TypeDirect::U32);
}

std::shared_ptr<IR_TypeDirect> IR_TypeDirect::getI64() {
    return getStaticType(6, IR_TypeDirect::I64);
}

std::shared_ptr<IR_TypeDirect> IR_TypeDirect::getU64() {
    return getStaticType(7, IR_TypeDirect::U64);
}

std::shared_ptr<IR_TypeDirect> IR_TypeDirect::getF32() {
    return getStaticType(8, IR_TypeDirect::F32);
}

std::shared_ptr<IR_TypeDirect> IR_TypeDirect::getF64() {
    return getStaticType(9, IR_TypeDirect::F64);
}


// IR_TypeStruct

IR_TypeStruct::StructField::StructField(string_id_t ident, std::shared_ptr<IR_Type> type, int index)
        : fieldName(ident), irType(std::move(type)), index(index) {}

bool IR_TypeStruct::equal(IR_Type const &rhs) const {
    if (auto rtype = rhs.castType<IR_TypeStruct>())
        return structId == rtype->structId;
    return false;
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

std::string IR_TypeStruct::to_string() const {
    return fmt::format("struct({})", structId);
}


// IR_TypePtr

bool IR_TypePtr::equal(const IR_Type &rhs) const {
    if (auto rtype = rhs.castType<IR_TypePtr>()) {
        return is_const == rtype->is_const && is_restrict == rtype->is_restrict &&
            is_volatile == rtype->is_volatile && this->child->equal(*rtype->child);
    }
    return false;    
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

std::string IR_TypePtr::to_string() const {
    return fmt::format("ptr< {} >", child->to_string());
}

// IR_TypeFunc

bool IR_TypeFunc::equal(const IR_Type &rhs) const {
    auto rtype = rhs.castType<IR_TypeFunc>();
    if (!rtype)
        return false;
    if (isVariadic != rtype->isVariadic || !ret->equal(*rtype->ret) || args.size() != rtype->args.size())
        return false;
    for (size_t i = 0; i < args.size(); i++) {
        if (!args[i]->equal(*rtype->args[i]))
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
    return 8; // This is bad
}

std::string IR_TypeFunc::to_string() const {
    std::stringstream ss;
    ss << "fun< " << ret->to_string() << " : ";
    for (auto const &arg : args)
        ss << arg->to_string() << " ";
    if (isVariadic)
        ss << "... ";
    ss << ">";
    return ss.str();
}


// IR_TypeArray

bool IR_TypeArray::equal(IR_Type const &rhs) const {
    if (auto rtype = rhs.castType<IR_TypeArray>())
        return size == rtype->size && child->equal(*rtype->child);
    return false;
}

std::shared_ptr<IR_Type> IR_TypeArray::copy() const {
    return std::make_shared<IR_TypeArray>(child->copy(), size);
}

int IR_TypeArray::getBytesSize() const {
    return static_cast<int>(size) * child->getBytesSize();
}

std::string IR_TypeArray::to_string() const {
    return fmt::format("array< {} * {} >", size, child->to_string());
}
