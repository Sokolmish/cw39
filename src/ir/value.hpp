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

    bool operator==(IRval const &oth) const;

    std::strong_ordering operator<=>(IRval const &oth) const;

    [[nodiscard]] static IRval createVal(std::shared_ptr<IR_Type> type, union_type v);
    [[nodiscard]] static IRval createReg(std::shared_ptr<IR_Type> type, uint64_t id);
    [[nodiscard]] static IRval createFunArg(std::shared_ptr<IR_Type> type, uint64_t num);
    [[nodiscard]] static IRval createString(uint64_t num);
    [[nodiscard]] static IRval createGlobal(std::shared_ptr<IR_Type> globalType, uint64_t num);
    [[nodiscard]] static IRval createFunPtr(std::shared_ptr<IR_Type> funPtrType, uint64_t num);
    [[nodiscard]] static IRval createUndef(std::shared_ptr<IR_Type> type);
    [[nodiscard]] static IRval createZeroinit(std::shared_ptr<IR_Type> type);
    [[nodiscard]] static IRval createAggregate(std::shared_ptr<IR_Type> type, std::vector<IRval> vals);

    [[nodiscard]] static IRval createDefault(std::shared_ptr<IR_Type> type);

    ValueClass getValueClass() const;
    std::shared_ptr<IR_Type> const& getType() const;
    union_type const& getVal() const;

    /** Check if value is constant, zeroinit or aggregate (always constant) */
    bool isConstant() const;

    bool isVReg() const;
    bool isGlobal() const;

    [[nodiscard]] std::string to_string() const;
    [[nodiscard]] std::string to_reg_name() const;

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
