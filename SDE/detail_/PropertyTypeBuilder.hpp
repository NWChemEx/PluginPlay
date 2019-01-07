#pragma once
#include <array>

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

    /// The current number of fields in the API
    static constexpr std::size_t nfields = sizeof...(ErasedTypes);

public:
    using key_type        = std::string;
    using mapped_type     = ElementType;
    using tuple_of_fields = std::tuple<ErasedTypes...>;
    /// The type of an array holding @p nkeys keys
    template<std::size_t nkeys>
    using key_array_type = std::array<key_type, nkeys>;

    /// The type of an array holding @p nvalues mapped values
    template<std::size_t nvalues>
    using mapped_array_type = std::array<mapped_type, nvalues>;
    using size_type         = typename key_array_type<nfields>::size_type;

    PropertyTypeBuilder()                               = default;
    PropertyTypeBuilder(const PropertyTypeBuilder& rhs) = default;
    my_type& operator=(const my_type& rhs)         = default;
    PropertyTypeBuilder(PropertyTypeBuilder&& rhs) = default;
    my_type& operator=(my_type&& rhs) = default;
    PropertyTypeBuilder(key_array_type<nfields> keys,
                        mapped_array_type<nfields> values) noexcept :
      keys_(std::move(keys)),
      values_(std::move(values)) {}

    ~PropertyTypeBuilder() = default;

    template<typename T>
    auto add_field(key_type key) {
        ElementType elem;
        elem.template set_type<T>();
        auto temp_keys   = std::move(add_key_(std::move(key)));
        auto temp_values = std::move(add_value_(std::move(elem)));
        return new_field<T>{std::move(temp_keys), std::move(temp_values)};
    }

    static constexpr size_type size() { return nfields; }

    size_type position(const key_type& key) const {
        for(size_type i = 0; i < size(); ++i)
            if(keys_[i] == key) return i;
        throw std::out_of_range(std::string("Key ") + key + " not found.");
    }

    auto& operator[](const key_type& key) { return at(key); }
    const auto& operator[](const key_type& key) const { return at(key); }
    auto& at(const key_type& key) { return values_[position(key)]; }
    const auto& at(const key_type& key) const { return values_[position(key)]; }

private:
    auto add_key_(key_type key) noexcept {
        key_array_type<nfields + 1> temp;
        for(size_type i = 0; i < nfields; ++i) temp[i] = std::move(keys_[i]);
        temp[nfields] = std::move(key);
        return std::move(temp);
    }

    auto add_value_(mapped_type value) {
        mapped_array_type<nfields + 1> temp;
        for(size_type i = 0; i < nfields; ++i) temp[i] = std::move(values_[i]);
        temp[nfields] = std::move(value);
        return std::move(temp);
    }

    key_array_type<nfields> keys_;
    mapped_array_type<nfields> values_;
};

} // namespace detail_
} // namespace SDE
