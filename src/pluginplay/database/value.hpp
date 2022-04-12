#pragma once
#include <optional>
#include <utility>

namespace pluginplay::database {

template<typename T>
class Value {
public:
    using value_type      = T;
    using reference       = T&;
    using pointer         = T*;
    using const_reference = const T&;

    explicit Value(value_type v) : m_value_(std::move(v)), m_ptr_(nullptr) {}
    explicit Value(pointer p) : m_ptr_(p) {}

    reference get();
    const_reference get() const;

private:
    /// If we own the value it's in the optional
    std::optional<T> m_value_;

    /// If we alias it this is it's address
    pointer m_ptr_;
};

template<typename T>
typename Value<T>::reference Value<T>::get() {
    return m_value_ ? m_value_.value() : *m_ptr_;
}

template<typename T>
typename Value<T>::const_reference Value<T>::get() const {
    return m_value_ ? m_value_.value() : *m_ptr_;
}

template<typename T>
using ConstValue = Value<const T>;

} // namespace pluginplay::database
