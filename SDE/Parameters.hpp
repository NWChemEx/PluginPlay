#pragma once

namespace SDE {

class ModuleInputs {
public:
    value_type& at(const key_type& key) { return inputs_.at(key); }
    const value_type& at(const key_type& key) const { return inputs_.at(key); }

    template<typename T>
    auto value(const key_type& key) {
        at(key)
    }

    template<typename T, typename U>
    void change(key_type key, U&& new_value) {
        at(key).value<T>() = std::forward<U>(new_value);
    }

private:
    input_map inputs_;

    template<std::size_t... ArgI, typename... Args>
    void change_values_(std::index_sequence<ArgI...>, Args&&... args) {}
};

} // namespace SDE
