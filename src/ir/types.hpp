#ifndef __IR_TYPES_HPP__
#define __IR_TYPES_HPP__

#include <vector>
#include <memory>
#include "parser/common.h"

struct IR_StorageSpecifier {
    enum Specs { EXTERN, STATIC, AUTO, REGISTER } spec;

    IR_StorageSpecifier();
    IR_StorageSpecifier(Specs spec);
};

/**
 * Consider type as immutable value, that can be used by different CFGs simultaneously
 */
struct IR_Type {
    enum Type { DIRECT, TSTRUCT, POINTER, ARRAY, FUNCTION } type;

    IR_Type(IR_Type const &) = delete;
    IR_Type& operator=(IR_Type const &) = delete;

    explicit IR_Type(Type type);
    virtual ~IR_Type() = default;
    virtual bool equal(IR_Type const &rhs) const = 0;
    virtual std::shared_ptr<IR_Type> copy() const = 0;
    virtual int getBytesSize() const = 0;
};

struct IR_TypeDirect : public IR_Type {
    enum DirType {
        VOID, I8, U8, I32, U32, I64, U64, F32
    } spec;

    IR_TypeDirect(DirType spec);
    bool equal(IR_Type const &rhs) const override;
    std::shared_ptr<IR_Type> copy() const override;

    [[nodiscard]] bool isInteger() const;
    [[nodiscard]] bool isFloat() const;
    [[nodiscard]] bool isSigned() const;
    [[nodiscard]] bool isUnsigned() const;
    [[nodiscard]] int getBytesSize() const override;

    static std::shared_ptr<IR_TypeDirect> type_void;
    static std::shared_ptr<IR_TypeDirect> type_i8;
    static std::shared_ptr<IR_TypeDirect> type_u8;
    static std::shared_ptr<IR_TypeDirect> type_i32;
    static std::shared_ptr<IR_TypeDirect> type_u32;
    static std::shared_ptr<IR_TypeDirect> type_i64;
    static std::shared_ptr<IR_TypeDirect> type_u64;
    static std::shared_ptr<IR_TypeDirect> type_f32;
};

struct IR_TypeStruct : IR_Type {
    struct StructField {
        string_id_t fieldName;
        std::shared_ptr<IR_Type> irType;
        int index;

        StructField(string_id_t ident, std::shared_ptr<IR_Type> type, int index);
    };

    string_id_t structId;
    std::vector<StructField> fields;

    IR_TypeStruct(string_id_t ident, std::vector<StructField> fields);
    bool equal(IR_Type const &rhs) const override;
    std::shared_ptr<IR_Type> copy() const override;
    int getBytesSize() const override;
    StructField const* getField(string_id_t id) const;
};

struct IR_TypePtr : public IR_Type {
    std::shared_ptr<IR_Type> child;
    bool is_const = false;
    bool is_restrict = false;
    bool is_volatile = false;

    explicit IR_TypePtr(std::shared_ptr<IR_Type> child);
    bool equal(IR_Type const &rhs) const override;
    std::shared_ptr<IR_Type> copy() const override;
    int getBytesSize() const override;
};

struct IR_TypeArray : public IR_Type {
    std::shared_ptr<IR_Type> child;
    uint64_t size;

    IR_TypeArray(std::shared_ptr<IR_Type> child, uint64_t size);
    bool equal(IR_Type const &rhs) const override;
    std::shared_ptr<IR_Type> copy() const override;
    int getBytesSize() const override;
};

struct IR_TypeFunc : public IR_Type {
    std::shared_ptr<IR_Type> ret;
    std::vector<std::shared_ptr<IR_Type>> args;
    bool isVariadic;

    explicit IR_TypeFunc(std::shared_ptr<IR_Type> ret);
    IR_TypeFunc(std::shared_ptr<IR_Type> ret, std::vector<std::shared_ptr<IR_Type>> args, bool variadic);
    bool equal(IR_Type const &rhs) const override;
    std::shared_ptr<IR_Type> copy() const override;
    int getBytesSize() const override;
};

#endif /* __IR_TYPES_HPP__ */