#ifndef UTILS_HPP_INCLUDED__
#define UTILS_HPP_INCLUDED__

#include <initializer_list>
#include <list>
#include <map>
#include <optional>
#include <variant>
#include <fmt/core.h>
#include <exception>


// TODO: use source_location

[[noreturn]] inline void internalError(const std::string &msg) {
    throw std::runtime_error(fmt::format("[INTERNAL] {}\n", msg));
}

[[noreturn]] inline void semanticError(const std::string &msg) {
    throw std::runtime_error(fmt::format("{}\n", msg));
}

[[noreturn]] inline void notImplemented(int line, const std::string &msg = "") {
    throw std::runtime_error(fmt::format("Not implemented ({}): {}\n", line, msg));
}

#define NOT_IMPLEMENTED(msg) notImplemented(__LINE__, __FILE__ " " msg)


template <typename T>
constexpr inline bool isInList(T const &val, std::initializer_list<T> const &lst) {
    for (auto const &e : lst)
        if (val == e)
            return true;
    return false;
}


template <typename K, typename V>
class VariablesStack {
private:
    std::list<std::map<K, V>> data;
    int level = 0;

public:
    VariablesStack() {
        increaseLevel();
    }

    void increaseLevel() {
        data.push_back(std::map<K, V>());
        level++;
    }

    void decreaseLevel() {
        if (level == 0)
            internalError("Attemt to decrease level of empty VariablesStack");
        data.pop_back();
        level--;
    }

    std::optional<V> get(K const &key) const {
        for (auto lay = data.rbegin(); lay != data.rend(); ++lay) {
            auto it = lay->find(key);
            if (it != lay->end())
                return it->second;
        }
        return {};
    }

    void put(K const &key, V const &val) {
        data.back().emplace(key, val);
    }

    bool hasOnTop(K const &key) {
        return data.back().contains(key);
    }
};


// https://stackoverflow.com/questions/61046705/casting-a-variant-to-super-set-variant-or-a-subset-variant
template <class... Args>
struct variant_cast_proxy {
    std::variant<Args...> v;

    template <class... ToArgs>
    operator std::variant<ToArgs...>() const {
        return std::visit(
                [](auto &&arg) -> std::variant<ToArgs...> {
                    static_assert((std::is_convertible_v<Args, std::variant<ToArgs...>> || ...),
                                  "No possible variant that could be converted exists");
                    if constexpr (std::is_convertible_v<decltype(arg), std::variant<ToArgs...>>)
                        return arg;
                    else
                        throw std::runtime_error("bad variant cast");
                }, v);
    }
};

template <class... Args>
auto variant_cast(const std::variant<Args...>& v) -> variant_cast_proxy<Args...> {
    return { v };
}

#endif /* UTILS_HPP_INCLUDED__ */
