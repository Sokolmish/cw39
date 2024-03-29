#ifndef IR_TYPES_HPP_INCLUDED__
#define IR_TYPES_HPP_INCLUDED__

#include <vector>
#include <memory>
#include "parser/common.hpp"

struct IR_StorageSpecifier {
    enum Specs { EXTERN, STATIC, AUTO, REGISTER } spec;

    IR_StorageSpecifier();
    IR_StorageSpecifier(Specs spec);
};

/**
 * Consider type as immutable value, that can be used by different CFGs simultaneously
 */
struct IR_Type {
    IR_Type(IR_Type const &) = delete;
    IR_Type& operator=(IR_Type const &) = delete;

    IR_Type() = default;

    virtual ~IR_Type() = default;
    virtual bool equal(IR_Type const &rhs) const = 0;
    virtual std::shared_ptr<IR_Type> copy() const = 0;
    virtual int getBytesSize() const = 0;
    virtual std::string to_string() const = 0;

    template <typename T>
    T* castType() {
        return dynamic_cast<T*>(this);
    }

    template <typename T>
    T const* castType() const {
        return dynamic_cast<T const*>(this);
    }
};

class IR_TypeDirect final : public IR_Type {
public:
    enum DirType {
        VOID, BOOL, I8, U8, I32, U32, I64, U64, F32, F64
    } spec;

    explicit IR_TypeDirect(DirType spec) : spec(spec) {}

    bool equal(IR_Type const &rhs) const override;
    std::shared_ptr<IR_Type> copy() const override;
    std::string to_string() const override;

    [[nodiscard]] bool isInteger() const;
    [[nodiscard]] bool isFloat() const;
    [[nodiscard]] bool isSigned() const;
    [[nodiscard]] bool isUnsigned() const;
    [[nodiscard]] int getBytesSize() const override;

    /** Returns new type, which is common for given ones */
    static std::shared_ptr<IR_Type> getCommonDirType(std::shared_ptr<IR_Type> const &a,
                                                     std::shared_ptr<IR_Type> const &b);

    static std::shared_ptr<IR_TypeDirect> getVoid();
    static std::shared_ptr<IR_TypeDirect> getI1();
    static std::shared_ptr<IR_TypeDirect> getI8();
    static std::shared_ptr<IR_TypeDirect> getU8();
    static std::shared_ptr<IR_TypeDirect> getI32();
    static std::shared_ptr<IR_TypeDirect> getU32();
    static std::shared_ptr<IR_TypeDirect> getI64();
    static std::shared_ptr<IR_TypeDirect> getU64();
    static std::shared_ptr<IR_TypeDirect> getF32();
    static std::shared_ptr<IR_TypeDirect> getF64();

private:
    static std::shared_ptr<IR_TypeDirect> getStaticType(size_t index, DirType type);
    static std::shared_ptr<IR_TypeDirect> staticTypes[10];
};

struct IR_TypeStruct final : IR_Type {
    struct StructField {
        string_id_t fieldName;
        std::shared_ptr<IR_Type> irType;
        int index;

        StructField(string_id_t ident, std::shared_ptr<IR_Type> type, int index);
    };

    string_id_t structId;
    std::vector<StructField> fields;

    IR_TypeStruct(string_id_t ident, std::vector<StructField> fields)
            : structId(ident), fields(std::move(fields)) {}

    bool equal(IR_Type const &rhs) const override;
    std::shared_ptr<IR_Type> copy() const override;
    int getBytesSize() const override;
    std::string to_string() const override;

    StructField const* getField(string_id_t id) const;
};

struct IR_TypePtr final : public IR_Type {
    std::shared_ptr<IR_Type> child;
    bool is_const = false;
    bool is_restrict = false;
    bool is_volatile = false;

    explicit IR_TypePtr(std::shared_ptr<IR_Type> child) : child(std::move(child)) {}

    bool equal(IR_Type const &rhs) const override;
    std::shared_ptr<IR_Type> copy() const override;
    int getBytesSize() const override;
    std::string to_string() const override;
};

struct IR_TypeArray final : public IR_Type {
    std::shared_ptr<IR_Type> child;
    uint64_t size;

    IR_TypeArray(std::shared_ptr<IR_Type> child, uint64_t size)
            : child(std::move(child)), size(size) {}

    bool equal(IR_Type const &rhs) const override;
    std::shared_ptr<IR_Type> copy() const override;
    int getBytesSize() const override;
    std::string to_string() const override;
};

struct IR_TypeFunc final : public IR_Type {
    std::shared_ptr<IR_Type> ret;
    std::vector<std::shared_ptr<IR_Type>> args;
    bool isVariadic;

    explicit IR_TypeFunc(std::shared_ptr<IR_Type> ret)
            : ret(std::move(ret)), args(), isVariadic(false) {}

    IR_TypeFunc(std::shared_ptr<IR_Type> ret, std::vector<std::shared_ptr<IR_Type>> args, bool variadic)
            : ret(std::move(ret)), args(std::move(args)), isVariadic(variadic) {}

    bool equal(IR_Type const &rhs) const override;
    std::shared_ptr<IR_Type> copy() const override;
    int getBytesSize() const override;
    std::string to_string() const override;
};

#endif /* IR_TYPES_HPP_INCLUDED__ */
