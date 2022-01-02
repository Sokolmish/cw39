#ifndef __IR_VALUE_HPP__
#define __IR_VALUE_HPP__

#include <memory>
#include <variant>
#include <optional>
#include "types.hpp"


class IRval {
public:
    typedef std::variant<
            uint8_t, int8_t, uint32_t, int32_t, uint64_t, int64_t,
            float, double> union_type;

    enum ValueClass {
        VAL, VREG, GLOBAL, STRING, FUN_PARAM, FUN_PTR, UNDEF
    } valClass;

    std::optional<int> version = {};

    IRval(std::shared_ptr<IR_Type> type, ValueClass vclass, union_type v);
    IRval copy() const;

    // TODO: clear versioned and unversioned comparsion
    bool operator==(IRval const &oth) const;

    static bool less(const IRval &a, const IRval &b);
    struct Comparator {
        bool operator()(const IRval& a, const IRval& b) const;
    };

    static bool lessVersions(const IRval &a, const IRval &b);
    struct ComparatorVersions {
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

    std::shared_ptr<IR_Type> const& getType() const;
    bool isConstant() const;
    bool isVReg() const;
    bool isGlobal() const;
    union_type const& getVal() const;
    std::string to_string() const;
    std::string to_reg_name() const;

    template <class T>
    T castValTo() const {
        return std::visit([](auto const &arg) -> T {
            return static_cast<T>(arg);
        }, val);
    }

private:
    std::shared_ptr<IR_Type> type;
    union_type val;
};

#endif /* __IR_VALUE_HPP__ */
