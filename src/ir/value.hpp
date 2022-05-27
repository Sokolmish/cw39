#ifndef IR_VALUE_HPP_INCLUDED__
#define IR_VALUE_HPP_INCLUDED__

#include <memory>
#include <variant>
#include "types.hpp"
#include "utils.hpp"

class IRval {
public:
    using union_type = std::variant<
            uint8_t, int8_t, uint32_t, int32_t, uint64_t, int64_t,
            float, double>;

    enum ValueClass {
        VAL, VREG, GLOBAL, STRING, FUN_PARAM, FUN_PTR,
        UNDEF, ZEROINIT, AGGREGATE,
    };

    IRval copy() const;

    bool operator==(IRval const &oth) const {
        return valClass == oth.valClass && type->equal(*oth.type) && val == oth.val;
    }

    std::strong_ordering operator<=>(IRval const &oth) const {
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

    static IRval createVal(std::shared_ptr<IR_Type> type, union_type v);
    static IRval createReg(std::shared_ptr<IR_Type> type, uint64_t id);
    static IRval createFunArg(std::shared_ptr<IR_Type> type, uint64_t num);
    static IRval createString(uint64_t num);
    static IRval createGlobal(std::shared_ptr<IR_Type> globalType, uint64_t num);
    static IRval createFunPtr(std::shared_ptr<IR_Type> funPtrType, uint64_t num);
    static IRval createUndef(std::shared_ptr<IR_Type> type);
    static IRval createZeroinit(std::shared_ptr<IR_Type> type);
    static IRval createAggregate(std::shared_ptr<IR_Type> type, std::vector<IRval> vals);

    static IRval createDefault(std::shared_ptr<IR_Type> type);

    ValueClass getValueClass() const {
        return valClass;
    }

    std::shared_ptr<IR_Type> const& getType() const {
        return type;
    }

    union_type const& getVal() const {
        return val;
    }

    bool isConstant() const {
        return isInList(valClass, IRval::VAL, IRval::ZEROINIT, IRval::AGGREGATE);
    }

    bool isVReg() const {
        return valClass == IRval::VREG;
    }

    bool isGlobal() const {
        return valClass == IRval::GLOBAL;
    }

    bool isFunParam() const {
        return valClass == IRval::FUN_PARAM;
    }

    std::string to_string() const;
    std::string to_reg_name() const;

    template <class T>
    T castValTo() const {
        if (valClass == IRval::AGGREGATE)
            throw cw39_internal_error("Cannot cast aggregate value");
        return std::visit([](auto const &arg) -> T {
            return static_cast<T>(arg);
        }, val);
    }

    std::vector<IRval> const& getAggregateVal() const;


private:
    ValueClass valClass = VAL; // ValueClass(0ULL)
    std::shared_ptr<IR_Type> type;

    // Aggregate vals is not part of union type for convenience reasons
    union_type val;
    std::vector<IRval> aggregateVals; // TODO: optional

    IRval(ValueClass vclass, std::shared_ptr<IR_Type> type, IRval::union_type v);
    IRval(ValueClass vclass, std::shared_ptr<IR_Type> type, std::vector<IRval> vals);
};

#endif /* IR_VALUE_HPP_INCLUDED__ */
