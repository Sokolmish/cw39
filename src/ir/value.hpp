#ifndef __IR_VALUE_HPP__
#define __IR_VALUE_HPP__

#include <memory>
#include <variant>
#include <optional>
#include "types.hpp"


class IRval {
public:
    using union_type = std::variant<
            uint8_t, int8_t, uint32_t, int32_t, uint64_t, int64_t,
            float, double>;

    enum ValueClass {
        VAL, VREG, GLOBAL, STRING, FUN_PARAM, FUN_PTR, UNDEF
    };

    IRval(std::shared_ptr<IR_Type> type, ValueClass vclass, union_type v);
    IRval copy() const;

    bool equal(IRval const &oth) const;
    /** Consider same values with different versions equal */
    bool equalIgnoreVers(IRval const &oth) const;

    static bool lessIgnoreVers(const IRval &a, const IRval &b);
    struct ComparatorIgnoreVers {
        bool operator()(const IRval& a, const IRval& b) const;
    };

    static bool less(const IRval &a, const IRval &b);
    struct Comparator {
        bool operator()(const IRval& a, const IRval& b) const;
    };

    [[nodiscard]] static IRval createVal(std::shared_ptr<IR_Type> type, union_type v);
    [[nodiscard]] static IRval createReg(std::shared_ptr<IR_Type> type, uint64_t id);
    [[nodiscard]] static IRval createFunArg(std::shared_ptr<IR_Type> type, uint64_t num);
    [[nodiscard]] static IRval createString(uint64_t num);
    [[nodiscard]] static IRval createGlobal(std::shared_ptr<IR_Type> globalType, uint64_t num);
    [[nodiscard]] static IRval createFunPtr(std::shared_ptr<IR_Type> funPtrType, uint64_t num);
    [[nodiscard]] static IRval createUndef(std::shared_ptr<IR_Type> type);

    [[nodiscard]] static IRval createDefault(std::shared_ptr<IR_Type> type);

    ValueClass getValueClass() const;
    std::shared_ptr<IR_Type> const& getType() const;
    bool isConstant() const;
    bool isVReg() const;
    bool isGlobal() const;
    union_type const& getVal() const;

    [[nodiscard]] std::string to_string() const;
    [[nodiscard]] std::string to_reg_name() const;

    bool hasVersion() const;
    bool isUndefVersion() const;
    std::optional<int> getVersion() const;
    void setVersion(int vers);
    void dropVersion();

    template <class T>
    T castValTo() const {
        return std::visit([](auto const &arg) -> T {
            return static_cast<T>(arg);
        }, val);
    }

private:
    ValueClass valClass;
    std::shared_ptr<IR_Type> type;
    union_type val;
    std::optional<int> version = {};
};

#endif /* __IR_VALUE_HPP__ */
