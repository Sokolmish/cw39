#ifndef UTILS_HPP_INCLUDED__
#define UTILS_HPP_INCLUDED__

#include <variant>
#include <fmt/core.h>
#include <exception>
#include <source_location>
#include <experimental/source_location> // Because of CLion bug

class cw39_exception : public std::exception {
public:
    cw39_exception(std::string excClass, std::string loc, std::string msg);
    const char* what() const noexcept override;
    std::string prettyWhat() const;

private:
    std::string excClass;
    std::string loc;
    std::string msg;
};

class cw39_internal_error : public cw39_exception {
public:
    using source_location = std::source_location; // Because of CLion bug
    cw39_internal_error(std::string msg, source_location const &l = source_location::current());

private:
    std::string formLoc(source_location const &loc) const;
};

class cw39_not_implemented : public cw39_internal_error {
public:
    using source_location = std::source_location; // Because of CLion bug
    cw39_not_implemented(std::string const &msg, source_location const &l = source_location::current());
};

class cw39_error : public cw39_exception {
public:
    cw39_error(std::string msg);
};


template <typename T, typename... Us>
constexpr inline bool isInList(T const &val, Us const&&... elems) {
    return ((val == elems) || ...);
}


// https://stackoverflow.com/questions/61046705/casting-a-variant-to-super-set-variant-or-a-subset-variant
template <class... Args>
struct variant_cast_proxy {
    std::variant<Args...> v;

    template <class... ToArgs>
    operator std::variant<ToArgs...>() const {
        auto visitor = [](auto &&arg) -> std::variant<ToArgs...> {
            static_assert((std::is_convertible_v<Args, std::variant<ToArgs...>> || ...),
                          "No possible variant that could be converted exists");
            if constexpr (std::is_convertible_v<decltype(arg), std::variant<ToArgs...>>)
                return arg;
            else
                throw std::logic_error("Bad variant cast");
        };
        return std::visit(std::move(visitor), v);
    }
};

template <class... Args>
auto variant_cast(const std::variant<Args...>& v) -> variant_cast_proxy<Args...> {
    return { v };
}

#endif /* UTILS_HPP_INCLUDED__ */
