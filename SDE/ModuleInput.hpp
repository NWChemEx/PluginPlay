#pragma once
#include "SDE/detail_/SDEAny.hpp"
#include <functional>
#include <string>

namespace SDE {
namespace detail_ {
class ModuleInputPIMPL;
}

/** @brief Overloads equality operator for reference wrappers.
 *
 * @tparam T The type of the object in the reference wrapper
 * @param lhs
 * @param rhs
 * @return
 */
template<typename T>
bool operator==(const std::reference_wrapper<T> lhs,
                const std::reference_wrapper<T> rhs) {
    return lhs.get() == rhs.get();
}

class ModuleInput {
private:
    /// Helper type for declaring a reference wrapper around an object of type T
    template<typename T>
    using cref_wrapper = std::reference_wrapper<const std::decay_t<T>>;

    /// Helper type for deducing whether or not a type is a const reference
    template<typename T>
    using is_const_ref =
      std::conjunction<std::is_reference<T>,
                       std::is_const<std::remove_reference_t<T>>>;

public:
    /// Type of a check to apply to a value
    template<typename T>
    using validity_check = std::function<bool(const T&)>;

    /// The type we use for type-erasure
    using any_type = detail_::SDEAny;

    /// Type of a check that operates on a type-erased value
    using any_check = validity_check<any_type>;

    /// Type of the description
    using description_type = std::string;

    ModuleInput();
    ModuleInput(const ModuleInput& rhs);
    ModuleInput& operator=(const ModuleInput& rhs);
    ModuleInput(ModuleInput&& rhs) noexcept;
    ModuleInput& operator=(ModuleInput&& rhs) noexcept;
    ~ModuleInput() noexcept;

    //@{
    /** @name Getters
     *
     *  These are functions for accessing the state of the input.
     *
     */
    template<typename T>
    T value() {
        constexpr bool by_value = std::is_same_v<std::decay_t<T>, T>;
        if constexpr(is_const_ref<T>::value || by_value)
            return const_cast<const ModuleInput&>(*this).value<T>();
        /* Want it by reference. Normally the any cast will catch this and
         * throw an error; however, we have one edge case to worry about:
         * we're supposedly storing a const reference, but the user gave us the
         * object by value (hence we're actually storing the object by value).
         * This
         * is an implementation detail, so we fake the cast.
         */
        if(is_cref_) throw std::bad_cast();
        return detail_::SDEAnyCast<T>(get_());
    }
    template<typename T>
    T value() const {
        return is_actually_cref_ ? unwrap_cref<T>(get_()) :
                                   detail_::SDEAnyCast<T>(get_());
    }
    bool is_optional() const noexcept;
    bool is_transparent() const noexcept;
    const description_type& description() const noexcept;
    //@}

    //@{
    /** @name Setters
     *
     * @return All setters return the current object, with the state modified in
     *         the requested manner to promote chaining.
     */
    template<typename T>
    auto& set_type() {
        constexpr bool is_ref   = std::is_reference_v<T>;
        constexpr bool is_c_ref = is_const_ref<T>::value;
        static_assert(is_c_ref || !is_ref,
                      "Inputs should be read-only references or by value");
        is_cref_ = is_c_ref;
        set_type_(typeid(T));
        return add_type_check<T>();
    }

    template<typename T>
    auto& add_check(validity_check<T> check, description_type desc = "") {
        any_check temp = [check{std::move(check)}](const any_type& new_value) {
            return check(ModuleInput::unwrap_cref<T>(new_value));
        };
        return add_check_(std::move(temp), std::move(desc));
    }
    template<typename T>
    auto& change(T&& new_value) {
        constexpr bool is_value = std::is_same_v<T, std::decay_t<T>>;
        if(!is_valid(new_value))
            throw std::invalid_argument("Value has failed one or more checks");
        any_type da_any;

        /* Fun time. The SDE supports modules taking arguments in one of two
         * ways:
         * 1. By const reference (read-only)
         * 2. By value
         *
         * When setting the actual value there's no stipulation requiring the
         * user to provide us that value in the same form. Specifically they
         * can provide us the value:
         * 1. In read/write mode (by reference)
         * 2. In read-only mode (by constant reference)
         * 3. By value (constructed it in place)
         *
         * Of these three possible inputs, number 3 is the easiest. We basically
         * just take ownership of it. For the other types of input what we do
         * depends on how it's going to be used. If the value will be used in a
         * read-only manner we just make a reference wrapper around it,
         * otherwise we copy it.
         */
        if constexpr(is_value) { // User gave us the input by value
            da_any            = wrap_value(std::forward<T>(new_value));
            is_actually_cref_ = false;
        } else {
            if(is_cref_) {
                da_any            = wrap_cref(std::forward<T>(new_value));
                is_actually_cref_ = true;
            } else {
                da_any = wrap_value(std::forward<T>(new_value));
            }
        }
        change_(da_any);
        return *this;
    }
    template<typename T>
    auto& set_default(T&& new_value) {
        return change(std::forward<T>(new_value));
    }
    ModuleInput& set_description(description_type desc) noexcept;
    ModuleInput& make_optional() noexcept;
    ModuleInput& make_required() noexcept;
    ModuleInput& make_transparent() noexcept;
    ModuleInput& make_opaque() noexcept;
    //@}

    template<typename T>
    bool is_valid(T&& new_value) const {
        return is_valid_(wrap_cref(std::forward<T>(new_value)));
    }

    void hash(Hasher& h) const;

private:
    any_type& get_() {
        const auto& temp = const_cast<const ModuleInput&>(*this).get_();
        return const_cast<any_type&>(temp);
    }
    const any_type& get_() const;

    template<typename T>
    auto& add_type_check() {
        any_check check = [](const any_type& new_value) {
            return new_value.type() == typeid(cref_wrapper<T>);
        };
        return add_check_(std::move(check), "Type Check");
    }

    template<typename T>
    static T unwrap_cref(const any_type& the_value) {
        return detail_::SDEAnyCast<cref_wrapper<T>>(the_value);
    }

    template<typename T>
    static auto wrap_cref(T&& new_value) {
        return detail_::make_SDEAny<cref_wrapper<T>>(
          std::cref(std::forward<T>(new_value)));
    }

    template<typename T>
    static auto wrap_value(T&& new_value) {
        using clean_T = std::decay_t<T>;
        return detail_::make_SDEAny<clean_T>(std::forward<T>(new_value));
    }

    void change_(any_type new_value);
    bool is_valid_(const any_type& new_value) const;
    void set_type_(const std::type_info& type) noexcept;

    /**
     *
     * @param check
     * @param desc
     * @return
     * @throws std::bad_alloc if there is insufficient memory to generate the
     *         description (if one is not provided) or the storage element.
     *         Strong throw guarantee.
     */
    ModuleInput& add_check_(any_check check, description_type desc);

    bool is_cref_          = false;
    bool is_actually_cref_ = false;
    std::unique_ptr<detail_::ModuleInputPIMPL> pimpl_;
};

} // namespace SDE
