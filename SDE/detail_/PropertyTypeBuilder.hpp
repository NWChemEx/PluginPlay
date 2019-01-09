#pragma once
#include <array>
#include <functional>

namespace SDE {
namespace detail_ {

template<typename ElementType, typename... ErasedTypes>
class PropertyTypeBuilder {
private:
    /// Type resulting from adding a new field of type T
    template<typename T>
    using new_field = PropertyTypeBuilder<ElementType, ErasedTypes..., T>;

    /// Type of this class
    using my_type = PropertyTypeBuilder<ElementType, ErasedTypes...>;

public:
    using size_type   = std::size_t;
    using key_type    = std::string;
    using mapped_type = ElementType;
    using value_type  = std::pair<key_type, mapped_type>;
    /// The current number of fields in the API
    static constexpr std::size_t nfields = sizeof...(ErasedTypes);
    using tuple_of_fields                = std::tuple<ErasedTypes...>;

    /// The type of an array holding @p N fields
    template<std::size_t N>
    using array_type = std::array<value_type, N>;

    using iterator       = typename array_type<nfields>::iterator;
    using const_iterator = typename array_type<nfields>::const_iterator;

    PropertyTypeBuilder()                               = default;
    PropertyTypeBuilder(const PropertyTypeBuilder& rhs) = default;
    my_type& operator=(const my_type& rhs)         = default;
    PropertyTypeBuilder(PropertyTypeBuilder&& rhs) = default;
    my_type& operator=(my_type&& rhs) = default;
    PropertyTypeBuilder(array_type<nfields> values) noexcept :
      values_(std::move(values)) {}

    ~PropertyTypeBuilder() = default;

    template<typename T>
    auto add_field(key_type key) {
        ElementType elem;
        elem.template set_type<T>();
        auto temp_values = add_field_(std::move(key), std::move(elem));
        return new_field<T>(std::move(temp_values));
    }

    static constexpr size_type size() { return nfields; }
    auto& operator[](const key_type& key) { return at(key); }
    const auto& operator[](const key_type& key) const { return at(key); }
    auto& at(const key_type& key) {
        const auto& temp = const_cast<const my_type&>(*this).at(key);
        return const_cast<mapped_type&>(temp.second);
    }
    const auto& at(const key_type& key) const {
        return values_.at(position_(key));
    }

    iterator begin() { return values_.begin(); }
    const_iterator begin() const { return values_.begin(); }
    const_iterator cbegin() const { return begin(); }
    iterator end() { return values_.end(); }
    const_iterator end() const { return values_.end(); }
    const_iterator cend() const { return end(); }

private:
    auto add_field_(key_type key, mapped_type value) noexcept {
        array_type<nfields + 1> temp;
        for(size_type i = 0; i < nfields; ++i) temp[i] = std::move(values_[i]);
        temp[nfields] = std::make_pair(std::move(key), std::move(value));
        return temp;
    }

    size_type position_(const key_type& key) const {
        for(size_type i = 0; i < size(); ++i)
            if(values_[i].first == key) return i;
        throw std::out_of_range(std::string("Key ") + key + " not found.");
    }

    array_type<nfields> values_;
};

} // namespace detail_
} // namespace SDE
