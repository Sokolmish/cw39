#include "value.hpp"
#include "utils.hpp"
#include <sstream>

IRval::IRval(ValueClass vclass, std::shared_ptr<IR_Type> type, IRval::union_type v) :
        valClass(vclass), type(std::move(type)), val(v) {}

IRval::IRval(IRval::ValueClass vclass, std::shared_ptr<IR_Type> type, std::vector<IRval> vals) :
        valClass(vclass), type(std::move(type)), aggregateVals(std::move(vals)) {}


IRval::ValueClass IRval::getValueClass() const {
    return valClass;
}

std::shared_ptr<IR_Type> const& IRval::getType() const {
    return type;
}

IRval IRval::createVal(std::shared_ptr<IR_Type> type, IRval::union_type v) {
    return IRval(IRval::VAL, std::move(type), v);
}

IRval IRval::createReg(std::shared_ptr<IR_Type> type, uint64_t id) {
    return IRval(IRval::VREG, std::move(type), id);
}

IRval IRval::createFunArg(std::shared_ptr<IR_Type> type, uint64_t num) {
    return IRval(IRval::FUN_PARAM, std::move(type), num);
}

IRval IRval::createString(uint64_t num) {
    auto strType = std::make_shared<IR_TypePtr>(IR_TypeDirect::getI8());
//    auto strPtrType = std::make_shared<IR_TypePtr>(strType);
//    strType->is_const = true;
    return IRval(IRval::STRING, std::move(strType), num);
}

IRval IRval::createGlobal(std::shared_ptr<IR_Type> globalType, uint64_t num) {
    return IRval(IRval::GLOBAL, std::move(globalType), num);
}

IRval IRval::createFunPtr(std::shared_ptr<IR_Type> funPtrType, uint64_t num) {
    return IRval(IRval::FUN_PTR, std::move(funPtrType), num);
}

IRval IRval::createUndef(std::shared_ptr<IR_Type> type) {
    return IRval(IRval::UNDEF, std::move(type), 0ULL);
}

IRval IRval::createAggregate(std::shared_ptr<IR_Type> type, std::vector<IRval> vals) {
    return IRval(IRval::AGGREGATE, std::move(type), std::move(vals));
}

IRval IRval::createZeroinit(std::shared_ptr<IR_Type> type) {
    return IRval(IRval::ZEROINIT, std::move(type), 0ULL);
}

IRval IRval::createDefault(std::shared_ptr<IR_Type> type) {
    if (type->type == IR_Type::POINTER)
        return IRval(IRval::VAL, std::move(type), static_cast<uint64_t>(0));

    if (type->type != IR_Type::DIRECT)
        generalError("Cannot create default value for such type");
    auto spec = dynamic_cast<IR_TypeDirect &>(*type).spec;

    switch (spec) {
        case IR_TypeDirect::BOOL:
            return IRval(IRval::VAL, std::move(type), static_cast<int8_t>(0));
        case IR_TypeDirect::I8:
            return IRval(IRval::VAL, std::move(type), static_cast<int8_t>(0));
        case IR_TypeDirect::U8:
            return IRval(IRval::VAL, std::move(type), static_cast<uint8_t>(0));
        case IR_TypeDirect::I32:
            return IRval(IRval::VAL, std::move(type), static_cast<int32_t>(0));
        case IR_TypeDirect::U32:
            return IRval(IRval::VAL, std::move(type), static_cast<uint32_t>(0));
        case IR_TypeDirect::I64:
            return IRval(IRval::VAL, std::move(type), static_cast<int64_t>(0));
        case IR_TypeDirect::U64:
            return IRval(IRval::VAL, std::move(type), static_cast<uint64_t>(0));
        case IR_TypeDirect::F32:
            return IRval(IRval::VAL, std::move(type), static_cast<float>(0));
        case IR_TypeDirect::F64:
            return IRval(IRval::VAL, std::move(type), static_cast<double>(0));
        case IR_TypeDirect::VOID:
            generalError("Cannot create value of type VOID");
    }
    throw;
}


bool IRval::isConstant() const {
    return isInList(valClass, IRval::VAL, IRval::ZEROINIT, IRval::AGGREGATE);
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

std::vector<IRval> const& IRval::getAggregateVal() const {
    if (valClass != IRval::AGGREGATE)
        throw std::runtime_error("Cannot get aggregate values from regular value");
    return aggregateVals;
}

std::string IRval::to_string() const {
    switch (valClass) {
        case IRval::VREG:
            return std::visit([](auto e) -> std::string {
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

        case IRval::FUN_PTR:
            return std::visit([](auto e) -> std::string {
                return fmt::format("@fun.{}", e);
            }, val);

        case IRval::UNDEF:
            return "undef";

        case IRval::ZEROINIT:
            return "zeroinit";

        case IRval::AGGREGATE: {
            if (aggregateVals.empty())
                return "{ }";
            std::stringstream ss;
            ss << "{ ";
            for (ssize_t i = 0; i < std::ssize(aggregateVals) - 1; i++)
                ss << aggregateVals.at(i).to_string() << ", ";
            ss << aggregateVals.back().to_string() << " }";
            return ss.str();
        }
    }
    internalError("Invalid type of IRval");
}

std::string IRval::to_reg_name() const {
    return std::visit([this](auto e) -> std::string {
        auto prefix = isVReg() ? "vr" : "W";
        return fmt::format("{}{}", prefix, e);
    }, val);
}

IRval IRval::copy() const {
    auto res = IRval(valClass, type->copy(), val);

    std::vector<IRval> newAggregateVals;
    for (const auto &v : aggregateVals)
        newAggregateVals.push_back(v.copy());
    res.aggregateVals = newAggregateVals;

    return res;
}

// TODO: comparison with zeroinit?
bool IRval::operator==(const IRval &oth) const {
    return valClass == oth.valClass && type->equal(*oth.type) && val == oth.val;
}

std::strong_ordering IRval::operator<=>(const IRval &oth) const {
    if (valClass < oth.valClass)
        return std::strong_ordering::less;
    else if (valClass > oth.valClass)
        return std::strong_ordering::greater;
    else [[likely]] {
        // NOTE: partial ordering (because of float), but in favor of speed it is ignored
        auto valOrd = val <=> oth.val;
        if (is_lt(valOrd)) // val < oth.val
            return std::strong_ordering::less;
        else if (is_gt(valOrd)) // val > oth.val
            return std::strong_ordering::greater;
        else
            return std::strong_ordering::equal;
    }
}
