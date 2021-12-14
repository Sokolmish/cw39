#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <initializer_list>
#include <list>
#include <map>
#include <optional>
#include <fmt/core.h>
#include <exception>


[[noreturn]] inline void semanticError(const std::string &msg) {
    throw std::runtime_error(fmt::format("{}\n", msg));
}
[[noreturn]] inline void notImplemented(int line, const std::string &msg = "") {
    throw std::runtime_error(fmt::format("Not implemented ({}): {}\n", line, msg));
}
#define NOT_IMPLEMENTED(msg) notImplemented(__LINE__, __FILE__ " " msg)


template <typename T>
inline bool isInList(T const &val, std::initializer_list<T> const &lst) {
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
            throw;
        data.pop_back();
        level--;
    }

    std::optional<V> get(K const &key) const {
        for (auto lay = data.rbegin(); lay != data.rend(); lay++) {
            auto it = lay->find(key);
            if (it != lay->end())
                return it->second;
        }
        return {};
    }

    void put(K const &key, V const &val) {
        data.back().template emplace(key, val);
//        data.back().insert({ key, val });
    }

    bool hasOnTop(K const &key) {
        return data.back().contains(key);
    }
};

#endif /* __UTILS_HPP__ */
