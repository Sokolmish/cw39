#include "value.hpp"
#include "utils.hpp"

IRval::IRval(std::shared_ptr<IR_Type> type, ValueClass vclass, IRval::union_type v) :
        valClass(vclass), type(type), val(v) {}

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
//    auto strPtrType = std::make_shared<IR_TypePtr>(strType);
//    strType->is_const = true;
    return IRval(strType, IRval::STRING, num);
}

IRval IRval::createGlobal(std::shared_ptr<IR_Type> globalType, uint64_t num) {
    return IRval(globalType, IRval::GLOBAL, num);
}


IRval IRval::createDefault(std::shared_ptr<IR_Type> type) {
    if (type->type == IR_Type::POINTER)
        return IRval(type, IRval::VAL, static_cast<uint64_t>(0));

    if (type->type != IR_Type::DIRECT)
        semanticError("Cannot create default value for such type");
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
        case IR_TypeDirect::F64:
            return IRval(type, IRval::VAL, static_cast<double>(0));
        case IR_TypeDirect::VOID:
            semanticError("Cannot create value of type VOID");
    }
    throw;
}


bool IRval::isConstant() const {
    return valClass == IRval::VAL;
}

bool IRval::isVReg() const {
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

std::string IRval::to_reg_name() const {
    return std::visit([this](auto e) -> std::string {
        auto prefix = isVReg() ? "vr" : "W";
        if (version)
            return fmt::format("{}{}.{}", prefix, e, *version);
        else
            return fmt::format("{}{}", prefix, e);
    }, val);
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

bool IRval::lessVersions(const IRval &a, const IRval &b) {
    if (a.valClass < b.valClass)
        return true;
    else if (a.valClass > b.valClass)
        return false;
    else {
        if (a.val < b.val)
            return true;
        else if (a.val > b.val)
            return false;
        else if (a.version.has_value() && b.version.has_value())
            return a.version < b.version;
        else
            return a.version < b.version;
    }
}

bool IRval::Comparator::operator()(const IRval &a, const IRval &b) const {
    return IRval::less(a, b);
}

bool IRval::ComparatorVersions::operator()(const IRval &a, const IRval &b) const {
    return IRval::lessVersions(a, b);
}
