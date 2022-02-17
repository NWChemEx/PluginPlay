#pragma once
#include "pluginplay/any/detail_/any_field_wrapper.hpp"
#include "pluginplay/any/detail_/any_result_base.hpp"

namespace pluginplay::detail_ {

/** @brief Implements the result-specific any.
 *
 *  This class implements the result-specific parts of the type-erased API of
 *  `AnyResultBase` used by `AnyResult` to interact with the type-erased value.
 *
 *  N.B., this class includes several static_asserts to ensure that @p T adheres
 *  to being an uqualified type.
 *
 *  @tparam T The type that this wrapper wraps. Expected to be an unqualified
 *            (*i.e.*, no cv-qualifiers and not a pointer/reference) type.
 */
template<typename T>
class AnyResultWrapper : public AnyFieldWrapper<T, AnyResultBase> {
private:
    /// Type this class derives from
    using base_type = AnyFieldWrapper<T, AnyResultBase>;

    /// Type of this class, including the template parameter
    using my_type = AnyResultWrapper<T>;

    /// Is true if @p U is a different type than this class's type
    template<typename U>
    static constexpr bool not_my_type = !std::is_same_v<my_type, U>;

    /// Results in type @p V only when @p U is different than this class's type
    template<typename U, typename V = void>
    using enable_if_not_me_t = std::enable_if_t<not_my_type<U>, V>;

public:
    /// Type used for returing this class via the base class
    using result_base_pointer = AnyResultBase::result_base_pointer;

    /** @brief Creates a new AnyResultWrapper by wrapping the provided value.
     *
     *  AnyResult instances own the value they are wrapping. Hence this ctor
     *  will deep copy the value provided to it unless the value is an rvalue
     *  reference (at which point it will take ownership of the value).
     *
     *  N.B. This function will fail to compile with an error along the lines of
     *  no matching function for call to `std::any::any(...)` if @p U is not
     *  implicitly convertible to type @p T.
     *
     *  @tparam U The type of the object we are wrapping. Must be implicitly
     *            convertible to @p T.
     *  @tparam <anonymous> Used to disable the function, via SFINAE, when @p U
     *                      is an instance of this class. In that case users are
     *                      presumably looking for the copy ctor.
     *
     * @param[in] u The object we are wrapping in the AnyResultWrapper.
     */
    template<typename U, typename = enable_if_not_me_t<std::decay_t<U>>>
    explicit AnyResultWrapper(U&& u);

    /** @brief Initializes this instance with a deep copy of another instance.
     *
     *
     *  @param[in] other The instance we are copying.
     */
    AnyResultWrapper(const AnyResultWrapper& other) : base_type(other) {}

protected:
    /// Adds type check to AnyFieldWrapper's implementation
    bool are_equal_(const AnyFieldBase& rhs) const noexcept override;

private:
    /// Implements clone, by calling the copy ctor.
    result_base_pointer clone_() const override {
        return std::make_unique<my_type>(*this);
    }
};

// -- Inline Implementations ---------------------------------------------------

template<typename T>
template<typename U, typename>
AnyResultWrapper<T>::AnyResultWrapper(U&& u) :
  base_type(std::make_any<T>(std::forward<U>(u))) {
    constexpr bool is_clean_t = std::is_same_v<T, std::decay_t<T>>;
    static_assert(is_clean_t,
                  "Template parameter for results must be unqualified");
}

template<typename T>
bool AnyResultWrapper<T>::are_equal_(const AnyFieldBase& rhs) const noexcept {
    // As long as no state is added after AnyFieldWrapper the base check is
    // sufficient
    if(!base_type::are_equal_(rhs)) return false;

    // but to be on the safe side, we make sure the types are the same here too
    auto prhs = dynamic_cast<const my_type*>(&rhs);
    return prhs != nullptr;
}

} // namespace pluginplay::detail_
