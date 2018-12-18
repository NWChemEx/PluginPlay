#pragma once

namespace SDE {
namespace detail_ {
class ComputedPropertiesPIMPL;
} // namespace detail_



/** @brief Class responsible for managing the returns of a module.
 *
 *  This class is designed to serve as an associative map between the properties
 *  computed by a module and a descriptive key.
 *
 */
class ComputedProperties {
public:
    using size_type = std::size_t;
    using key_type = std::string;
    using value_type = Property;

    //@{
    /** @name Ctors and Assignment Operators
     *
     *  1) Default Ctor
     *  2 and 3) Copy ctor/assignment operator.
     *  4 and 5) Move ctor/assignment operator.
     *
     *  @throw std::bad_alloc Thrown by all ctor/assignment operators aside
     *         from 4 and 5 if there is insufficient memory to create the new
     *         instance. Strong throw guarantee.
     *  @throw None Move ctor/assignment operator are no throw guarantee.
     *
     */
    ComputedProperties();
    ComputedProperties(const ComputedProperties& rhs);
    ComputedProperties& operator=(const ComputedProperties& rhs) {
        return *this = std::move(ComputedProperties(rhs));
    }
    ComputedProperties(ComputedProperties&& rhs)noexcept;
    ComputedProperties& operator=(ComputedProperties&& rhs)noexcept;
    //@}

    ~ComputedProperties()noexcept;

    size_type count(const key_type& key)const noexcept;
    size_type size() const noexcept;

    value_type get(const key_type& key) const;

    template<typename T>
    const T& at(const key_type& key) const {
        return get(key).value<T>();
    }

    template<typename T>
    void insert(const key_type& key, T&& value){
        if constexpr(std::are_same_v<value_type, std::decay_t<T>>) {
            insert_(key, std::forward<T>(value));
        }
        else {
            Property prop;
            prop.put(std::forward<T>(value));
            insert_(key, std::move(prop));
        }
    }
private:
    void insert_(const key_type& key, value_type value);

};

} // namespace SDE
