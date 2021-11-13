#pragma once
#include "pluginplay/detail_/any_field_wrapper.hpp"
#include "pluginplay/hasher.hpp"
#include "pluginplay/types.hpp"
#include "pluginplay/utility.hpp"
#include <any>
#include <memory>
#include <utilities/printing/print_stl.hpp>

namespace pluginplay::detail_ {
/// Forward declare class that will implement the API
template<typename T>
class AnyInputWrapper;

/** @brief The class responsible for holding the type-erased instance.
 *
 * The AnyInputWrapper class holds the wrapped instance for the
 * AnyInputWrapperBase class.  It also is responsible for implementing all
 * of the abstract methods
 *
 *
 * @tparam T The type of the instance to wrap. Must be copyable, hashable,
 * and have operator== defined.
 */
template<typename T>
class AnyInputWrapper : public AnyFieldWrapperBase {
private:
    using base_type = AnyFieldWrapperBase;

    template<typename U>
    using enable_if_not_pluginplay_any_t =
      typename base_type::template enable_if_not_wrapper_t<U>;

public:
    /**
     * @brief Creates a new AnyInputWrapper with the provided value
     *
     * @param[in] value The value we are wrapping.
     *
     * @throw ??? If @p T's ctor throws when @p value is forwarded to it.
     * Strong throw guarantee.
     */
    template<typename U, enable_if_not_wrapper_t<U> = 0>
    explicit AnyInputWrapper(U&& value) : base_type(std::forward<U>(value)) {}

protected:
    /** @brief Deep copies the type-erased value held in @p rhs.
     *
     *
     *  @param[in] rhs The instance to copy.
     *
     *  @throw ??? if the copying the value throws. Same guarantee.
     */
    AnyInputWrapper(const AnyInputWrapper<T>& rhs) = default;

    /** @brief Takes ownership of the type-erased value held in @p rhs.
     *
     *
     *  @param[in] rhs The instance to move from. After the move operation
     *                 @p rhs is in a valid, but otherwise undefined state.
     *
     *  @throw none No throw guarantee.
     */
    AnyInputWrapper(AnyInputWrapper<T>&&) noexcept = default;

    /** @brief Sets the current state to a deep copy of @p rhs.
     *
     *
     *  @param[in] rhs The instance to copy.
     *
     *  @return the current instance containing a deep copy of @p rhs's
     * state.
     *
     *  @throw ??? if the copying the value throws. Same guarantee.
     */
    AnyInputWrapper<T>& operator=(const AnyInputWrapper<T>&) = default;

    /** @brief Replaces the current state with the type-erased value held
     *        in @p rhs.
     *
     *  @param[in] rhs The instance to move from. After the move operation
     *                 @p rhs is in a valid, but otherwise undefined state.
     *
     *  @return The current instance set containing @p rhs's state.
     *
     *  @throw none No throw guarantee.
     */
    AnyInputWrapper<T>& operator=(AnyInputWrapper<T>&&) noexcept = default;

private:
    /// Type of a unique_ptr to the base
    using wrapper_ptr = typename base_type::wrapper_ptr;

    /// Type of the rtti
    using rtti_type = typename base_type::rtti_type;

    /** @brief Code factorization for casting to a read/write reference
     *
     *  @return The wrapped value in a read/write state
     *
     *  @throw none No throw guarantee (because we know the type).
     */
    T& value_() noexcept { return cast<T&>(); }

    /** @brief Code factorization for casting to a read-only reference
     *
     *  @return The wrapped value in a read-only state
     *
     *  @throw none No throw guarantee (because we know the type).
     */
    const T& value_() const noexcept { return cast<const T&>(); }

    /** @brief Deep copies the wrapped value into another
     *
     *  This function ultimately invokes the wrapped instances copy ctor.
     *
     *  @return A newly allocated AnyBase_ instance.
     *
     *  @throw ??? If @p T's copy constructor throws.  Same guarantee as
     *             T's copy constructor.
     */
    wrapper_ptr clone_() const override;

    /** @brief Returns the RTTI of the wrapped instance.
     *
     *  This implementation simply calls `typeid` on @p T and returns the
     *  result.
     *
     *  @return The RTTI of the wrapped instance.
     *
     * @throws None. No throw guarantee.
     */
    rtti_type type_() const noexcept override { return typeid(T); }

    /** @brief Converts the contents to a string.
     *
     *  This function works by using utilties::type_traits::IsPrintable to
     *  determine if type T can be printed. If it can be printed we then
     * pass it to a stringstream and return the result. Otherwise, we print
     * the mangled name and address.
     *
     *  @return A string representation of the wrapped value.
     *
     *  @throw std::bad_alloc if there is insufficient memory to allocate
     * the string. Strong throw guarantee.
     */
    std::string str_() const override;

    /** @brief Implements the equality comparison for the Any
     *
     *  Ultimately both operator== and operator!= will call this function
     * (the latter will negate the result).
     *
     *  @param[in] rhs The instance to compare against for equality.
     *
     *  @return true if the wrapped value compares equal to the value
     * wrapped in @p rhs and false otherwise.
     *
     *  @throw none No throw guarantee.
     */
    bool are_equal_(const AnyFieldWrapperBase& rhs) const noexcept override;

    /** @brief Implements hashing for the AnyBase_ class.
     *
     *  @param[in,out] h The hasher instance to use for the hashing.
     *
     *  @throws ??? if the wrapped instance's hash function throws.  Strong
     *              throw guarantee.
     */
    void hash_(Hasher& h) const override { h(value_()); }
};

//-------------------------------Implementations--------------------------------

template<typename U>
explicit AnyInputWrapper(U&& value)
  -> AnyInputWrapper<std::remove_reference_t<U>>;

template<typename T>
typename AnyInputWrapper<T>::wrapper_ptr AnyInputWrapper<T>::clone_() const {
    return std::make_unique<AnyInputWrapper<T>>(value_());
}

template<typename T>
std::string AnyInputWrapper<T>::str_() const {
    std::stringstream ss;
    using utilities::printing::operator<<;
    if constexpr(utilities::type_traits::is_printable_v<T>) {
        ss << value_();
    } else {
        ss << "<" << typeid(T).name() << " " << &value_() << ">";
    }
    return ss.str();
}

template<typename T>
bool AnyInputWrapper<T>::are_equal_(
  const AnyFieldWrapperBase& rhs) const noexcept {
    try {
        return value_() == rhs.cast<const T&>();
    } catch(...) {
        // Means the cast failed, so @p rhs holds a different type
        return false;
    }
}

} // namespace pluginplay::detail_
