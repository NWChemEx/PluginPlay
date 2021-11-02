#pragma once
#include "pluginplay/detail_/any_input_wrapper.hpp"
#include "pluginplay/utility.hpp"
#include <iostream>
#include <memory>
#include <type_traits>

namespace pluginplay::detail_ {

/** @brief The AnyInput class is capable of holding an instance of any type
 * in a type-safe manner.
 *
 *  This class is used extensively through the pluginplay to avoid needing to
 * know the types of inputs and results to a property type or module API. Under
 * the hood it works by wrapping an std::any and casting that any to implement
 * the additional functions which are part of the AnyInput's API, but not
 * the std::any's API.
 */
class AnyInput {
private:
    /// Trait to see if @p T is an AnyInput
    template<typename T>
    using is_any_input = std::is_same<AnyInput, std::decay_t<T>>;

    /// True if @p T is an AnyInput
    template<typename T>
    static constexpr bool is_any_input_v = is_any_input<T>::value;

    /// True if @p T is not an AnyInput
    template<typename T>
    static constexpr bool not_an_any_input_v = std::negation_v<is_any_input<T>>;

    /// Enables a function if @p T is an AnyInput
    template<typename T>
    using enable_if_any_input_t = std::enable_if_t<is_any_input_v<T>, int>;

    /// Enables a function if @p T is not an AnyInput
    template<typename T>
    using enable_if_not_an_any_input_t =
      std::enable_if_t<not_an_any_input_v<T>, int>;

public:
    /// The type of rtti returned by the `type` function
    using rtti_type = typename AnyInputWrapperBase::rtti_type;

    /// The type of the pointer holding the value
    using wrapper_ptr = typename AnyInputWrapperBase::wrapper_ptr;

    /** @brief Makes an empty AnyInput instance.
     *
     *  The resulting AnyInput instance wraps no object.  An object can be
     * added to this instance by calling the member function emplace, by
     * assigning to this instance another AnyInput instance (containing a
     * value), or by moving from another AnyInput instance containing a
     * value.
     *
     *  @throw None. No throw guarantee.
     */
    AnyInput() noexcept = default;

    /** @brief Used to construct an AnyInput instance holding a particular
     * value.
     *
     * This ctor forwards the provided value to the underlying std::any. If an
     * lvalue is provided a copy will be made.
     *
     * @tparam T The type of the instance to wrap. Must be copyable, hashable,
     *           and have operator== defined.
     * @tparam <anonymous> A dummy type used to implement SFINAE.
     *
     * @param[in] value The instance to wrap. It will be forwarded to the
     *                  underlying std::any.
     *
     *
     * @throws std::bad_alloc if memory allocation fails.  Strong throw
     *                        guarantee.
     * @throws ??? If the copy/move constructor of @p T throws. Same guarantee
     *             as @p T's ctor.
     */
    template<typename T, enable_if_not_an_any_input_t<T> = 0>
    explicit AnyInput(T&& value);

    /**
     * @brief Makes a new AnyInput instance by deep copying the instance
     * wrapped in another AnyInput instance.
     *
     * From the perspective of this class the copy ctor always makes a deep
     * copy. Whether it actually is a deep copy or not depends on the copy
     * constructor of the wrapped class.
     *
     * @param[in] rhs The AnyInput instance to copy.
     *
     * @throw std::bad_alloc if there is insufficient memory to copy the
     *                       instance stored in @p rhs.  Strong throw guarantee.
     * @throw ??? if the wrapped type's copy ctor throws.  Same guarantee as
     *            the wrapped type's copy ctor
     */
    AnyInput(const AnyInput& rhs) : m_ptr_(rhs.clone_()) {}

    /**
     * @brief Sets the current instance to a copy of another instance.
     *
     * This function will call the copy constructor of the object wrapped by
     * @p rhs and set the current instance's state to the result.  The state
     * before this call (if any) will be deleted.
     *
     * @param[in] rhs The instance to deep copy.
     *
     * @return The current instance containing a copy of @p rhs's state.
     *
     * @par Complexity:
     * Same as the wrapped type.
     *
     * @par Data Races:
     * The current state is modified, meaning attempts to concurrently access it
     * may lead to data races.  Similarly the contents of @p rhs are accessed
     * and concurrent modifications may lead to data races.
     *
     * @throws std::bad_alloc if there is insufficient memory to copy the
     * instance stored in @p rhs.  Strong throw guarantee.
     * @throws ??? if the wrapped type's constructor throws.  Strong throw
     * guarantee.
     */
    AnyInput& operator=(const AnyInput& r) {
        return *this = std::move(AnyInput(r));
    }

    /**
     * @brief Causes the current AnyInput instance to take ownership of
     * another instance.
     *
     * @param[in] rhs The AnyInput instance to take ownership of. After
     * calling this constructor @p rhs is in a valid, but undefined state.
     *
     * @par Complexity:
     * Constant.
     *
     * @par Data Races:
     * The contents of @p rhs are modified so attempts to concurrently access
     * @p rhs may result in data races.
     *
     * @throws None. No throw guarantee.
     */
    AnyInput(AnyInput&& rhs) noexcept = default;

    /**
     * @brief Sets the current instance to the state of another AnyInput
     * instance.
     *
     * @param[in] rhs The AnyInput instance we are taking the state of.
     * After this call @p rhs is in a valid, but otherwise undefined state.
     *
     * @par Complexity:
     * Constant.
     *
     * @par Data Races:
     * The contents of both this instance and @p rhs are modified.
     * Consequentially, attempts to concurrently access either instances' state
     * may yield a data race.
     *
     * @return The current instance possessing the state of @p rhs.
     * @throw None. No throw guarantee.
     */
    AnyInput& operator=(AnyInput&& rhs) noexcept = default;

    /** @brief Frees up the memory wrapped by the AnyInput instance.
     *
     *  This is a default dtor. After calling it all references to the
     *  underlying type-erased instance are no longer valid.
     *
     *  @throw None. No throw guarantee.
     */
    ~AnyInput() = default;

    /**
     * @brief Initializes the wrapped value by forwarding the provided arguments
     * to the wrapped value's constructor.
     *
     * This function can be used to change the instance wrapped inside this
     * object.  More specifically the arguments provided to this function are
     * forwarded to @p T's constructor, and the resulting instance becomes the
     * wrapped state (freeing up any existing state).  Note the arguments can be
     * a @p T instance in which case the copy or move constructor of @p T will
     * be invoked.
     *
     * This function is admittedly a bit intimidating until you see it in
     * action.  Let's say you have an AnyInput instance and you want it to
     * wrap a double, this could be done like:
     *
     * @code
     * AnyInput my_any_input;
     * double& wrapped_value = my_any_input.emplace<double>(3.14);
     * @endcode
     *
     * For a more complicated type, like an std::vector<double> we could instead
     * do:
     *
     * @code
     * AnyInput my_any_input;
     * std:vector<double> vec1{1.1, 1.2, 1.3};
     *
     * // Makes a copy of vec1
     * auto& wrapped_ve1 = my_any_input.emplace<std::vector<double>>(vec1);
     *
     * // ...makes vec1, inside the AnyInput, without the copy
     * auto& wrapped_v2 =
     * my_any_input.emplace<std::vector<double>>({1.1, 1.2, 1.3});
     * @endcode
     *
     * @tparam T The type of the object to wrap.
     * @tparam Args The types of the arguments to forward to the @p T's
     * constructor.
     *
     * @param[in] args The arguments to forward to @p T's constructor.
     *
     * @par Complexity:
     * Same as T's constructor.
     *
     * @par Data Races:
     * The state of the current instance is modified and a data race may occur
     * if the current instance is concurrently accessed.  Data races may also
     * be present for @p args upon forwarding to T's constructor depending on T.
     *
     * @return The wrapped instance by reference.
     * @throw std::bad_alloc if there is insufficient memory.  Strong throw
     * guarantee.
     * @throw ??? If @p T's constructor throws given @p args.  Same guarantee
     * as T's constructor.
     */
    template<typename T, typename... Args>
    T& emplace(Args&&... args);

    /** @brief Releases the wrapped value freeing up the memory associated with
     *         it.
     *
     *  AnyInput instances own the memory they are wrapping.  This member
     * function can be used to release the memory before the AnyInput
     * instance goes out of scope. Following normal RAII practices the
     * AnyInput will release any held memory in its dtor meaning users need
     * not call this function before letting an AnyInput instance go out of
     * scope.
     *
     * @throw None. No throw guarantee.
     */
    void reset() noexcept { m_ptr_.reset(); }

    /** @brief Swaps the states of two AnyInput instances.
     *
     *  After a call to this function, the current instance will contain
     *  @p rhs's state and @p rhs will contain the current instance's state.
     *
     * @param[in,out] rhs The AnyInput instance to swap contents with.
     * After this call @p rhs will contain the current instance's state.
     *
     * @throw None. No throw guarantee.
     *
     * @par Complexity constant.
     */
    void swap(AnyInput& rhs) noexcept { m_ptr_.swap(rhs.m_ptr_); }

    /** @brief Used to retrieve the value wrapped in the AnyInput.
     *
     *  The canonical way to retrieve the value from an AnyInput is to use
     *  AnyInputCast. This function is the implementation for the cast and
     * can be called directly if the user wants.
     *
     * @tparam T The type to cast the wrapped AnyInput instance to. Should
     * include cv qualifiers and include reference/pointer designations if so
     *         desired.
     *
     * @return The wrapped value as the requested type.
     *
     * @throws std::bad_any_cast if the instance does not contain a wrapped
     *                           value or if that wrapped value can not be
     *                           converted to @p T. Strong throw guarantee.
     */
    template<typename T>
    T cast();

    /** @brief Used to retrieve the value wrapped in a read-only AnyInput.
     *
     *  The canonical way to retrieve the value from an AnyInput is to use
     *  AnyInputCast. This function is the implementation for the cast and
     * can be called directly if the user wants. This overload will only allow
     * you to retrieve the value in a read-only state.
     *
     * @tparam T The type to cast the wrapped AnyInput instance to. Should
     * include cv qualifiers and include reference/pointer designations if so
     *         desired.
     *
     * @return The wrapped value as the requested type.
     *
     * @throws std::bad_any_cast if the instance does not contain a wrapped
     *                           value or if that wrapped value can not be
     *                           converted to @p T. Strong throw guarantee.
     */
    template<typename T>
    T cast() const;

    /** @brief Returns true if the current AnyInput instance is presently
     * wrapping a value.
     *
     *  An AnyInput can either be holding a type-erased value or not. This
     *  function is used to determine which it is.
     *
     * @return True if the current instance is holding a value and false
     *         otherwise.
     *
     * @throw None. No throw guarantee.
     *
     * @par Complexity:
     *      Constant.
     *
     */
    bool has_value() const noexcept { return m_ptr_ != nullptr; }

    template<typename T>
    bool is_convertible() const {
        return m_ptr_->is_convertible<T>();
    }

    /** @brief Returns the type of the wrapped instance.
     *
     *  This function returns the RTTI of the wrapped instance.  It should be
     *  noted that the representation of the RTTI is compiler specific and
     *  should be used with caution.  If the current instance is not wrapping
     *  anything the result will be typeid(void).
     *
     * @return The RTTI of the wrapped type.
     * @throw None. No throw guarantee.
     */
    rtti_type type() const noexcept;

    /** @brief Forwards the wrapped type to the provided Hasher.
     *
     *  @param[in,out] h A Hasher instance to use for the hashing.
     *
     *  @throws ??? if the wrapped instance's hash function throws.  Same
     *              throw guarantee as the wrapped instance's hash function.
     *
     * @par Complexity:
     *  Same as the complexity of hashing the wrapped type.
     */
    void hash(Hasher& h) const { h(m_ptr_); }

    /** @brief Creates a human-readable string representation of the wrapped
     *         instance.
     *
     *  This function ultimately works by printing "<empty AnyInput>" if
     * the current instance does not contain a value, or the result of
     * forwarding the wrapped value to `std::ostream::operator<<`. Overloads of
     * this operator for STL containers are leveraged.
     *
     *  @throw std::bad_alloc if there is insufficient memory to create the
     *                        string. Strong throw guarantee.
     *  @throw ??? if std::ostream<< throws when provided the wrapped value.
     *             Same guarantee as std::ostream<<.
     */
    std::string str() const;

    /** @brief Compares two AnyInput instances for equality.
     *
     * Two AnyInput instances are equal if they both hold a value (or do
     * not hold a value) and, in the case that they both hold a value, the
     * wrapped values compare equal.
     *
     * @param[in] rhs The instance to compare to
     *
     * @return true if the AnyInput instances are equal and false
     * otherwise.
     *
     * @throw None. All comparisons are no throw guarantee.
     */
    bool operator==(const AnyInput& rhs) const noexcept;

    /** @brief Compares two AnyInput instances to see if they are
     * different.
     *
     * Two AnyInput instances are equal if they both hold a value (or do
     * not hold a value) and, in the case that they both hold a value, the
     * wrapped values compare equal.
     *
     * @param[in] rhs The instance to compare to
     *
     * @return false if the AnyInput instances are equal and true
     * otherwise.
     *
     * @throw None. All comparisons are no throw guarantee.
     */
    bool operator!=(const AnyInput& rhs) const noexcept;

private:
    /** @brief Code factorization for copying the wrapper.
     *
     *  The wrapper is copied by calling its clone member. This function first
     *  ensures that we are holding a wrapper. If we are it then calls that
     *  wrapper's clone member. If we do not have a wrapper this function simply
     *  returns an empty pointer.
     *
     *  @return The wrapper
     *
     *  @throw std::bad_alloc if there is insufficient memory to allocate the
     *                        copy. Strong throw guarantee.
     *  @throw ??? If the value's copy ctor throws. Same guarantee
     *             as the value's copy ctor.
     */
    wrapper_ptr clone_() const;

    /** @brief Code factorization for emplace creation of the internal pointer.
     *
     *  This function creates a new @p T instance by forwarding the provided
     *  arguments to @p T's ctor. The resulting @p T is then moved into the
     *  internal wrapper.
     *
     * @tparam T The cv qualified type that this AnyInput instance will be
     *           wrapping.
     *
     * @tparam Args The types of the arguments that will be provided to @p T's
     *              ctor.
     *
     * @param[in] args The arguments to forward to @p T's ctor.
     *
     * @return A unique_ptr containing an AnyInputWrapperBase instance
     * which wraps the newly created @p T instance.
     *
     * @throws std::bad_alloc if there is insufficient memory to allocate a
     *                        new pluginplayWrapper<T> instance.  Strong throw
     *                        guarantee.
     * @throws ??? if @p T's ctor throws. Same guarantee as @p T's ctor.
     */
    template<typename T, typename... Args>
    static wrapper_ptr wrap_ptr_(Args&&... args);

    /// The object holding the actual value
    wrapper_ptr m_ptr_;
};

/** @brief Provides access to the value wrapped in an AnyInput instance.
 *
 * @relates AnyInput
 *
 * @tparam T The type of the value wrapped in the AnyInput instance.
 * @tparam U The type of the AnyInput instance itself. Should be at most
 *           cv-qualified AnyInput.
 *
 * @param[in] da_any_input The AnyInput instance to retrieve the value from.
 *
 * @return The value wrapped by @p da_any_input.
 *
 * @throw std::bad_any_cast if the value wrapped by @p da_any_input is not
 *                          convertible to type @p T or if @p da_any_input does
 * not contain a value.  Strong throw guarantee.
 *
 * @par Complexity: Constant.
 */
template<typename T, typename U>
T AnyInputCast(U&& da_any_input) {
    return da_any_input.template cast<T>();
}

/** @brief Makes an AnyInput instance by forwarding the arguments to the
 * wrapped instance's constructor.
 *
 *  @relates AnyInput
 *
 *  This is a convenience function for directly populating an AnyInput
 * instance so that the user does not have to first construct a temporary and
 * then forward that temporary to an AnyInput instance.
 *
 *  @tparam T The type of the object that the AnyInput should wrap.
 *  @tparam Args The types of the arguments that are being forwarded.
 *
 *  @param[in] args The values to forward to @p T's ctor.
 *
 *  @return An AnyInput instance containing a newly created wrapped @p T
 * instance.
 *
 *  @par Data Races:
 *  The values of @p args are forwarded to T's ctor and data races may occur if
 *  the values of args are concurrently modified.
 *
 *  @throw std::bad_alloc if there is insufficient memory for the AnyInput
 * to create the wrapper.  Strong throw guarantee.
 *  @throw ??? If @p T's constructor throws.  Same guarantee as T's constructor.
 *
 *  @par Complexity:
 *  Same as T's ctor taking @p args.
 *
 */
template<typename T, typename... Args>
AnyInput make_AnyInput(Args&&... args);

//--------------------------Implementations-------------------------------------

template<typename T, typename AnyInput::enable_if_not_an_any_input_t<T>>
AnyInput::AnyInput(T&& value) :
  m_ptr_(wrap_ptr_<std::conditional_t<std::is_reference_v<T>,
                                      std::remove_reference_t<T>, T>>(
    std::forward<T>(value))) {}

template<typename T, typename... Args>
T& AnyInput::emplace(Args&&... args) {
    wrap_ptr_<T>(std::forward<Args>(args)...).swap(m_ptr_);
    return m_ptr_->cast<T&>();
};

template<typename T>
T AnyInput::cast() {
    if(!has_value()) throw std::bad_any_cast();
    return m_ptr_->cast<T>();
}

template<typename T>
T AnyInput::cast() const {
    if(!has_value()) throw std::bad_any_cast();
    // Not sure if it's a compiler bug or I'm missing something, but for some
    // reason just doing ->cast<T>() calls the non-const version
    const auto* p = m_ptr_.get();
    return p->cast<T>();
}

inline typename AnyInput::rtti_type AnyInput::type() const noexcept {
    return has_value() ? m_ptr_->type() : typeid(void);
}

inline std::string AnyInput::str() const {
    if(has_value()) return m_ptr_->str();
    return "<empty AnyInput>";
}

inline bool AnyInput::operator==(const AnyInput& rhs) const noexcept {
    if(has_value() != rhs.has_value()) return false;
    if(!has_value()) return true;
    return (*m_ptr_) == (*rhs.m_ptr_);
}

inline bool AnyInput::operator!=(const AnyInput& rhs) const noexcept {
    return !((*this) == rhs);
}

inline typename AnyInput::wrapper_ptr AnyInput::clone_() const {
    return has_value() ? m_ptr_->clone() : wrapper_ptr{};
}

template<typename T, typename... Args>
typename AnyInput::wrapper_ptr AnyInput::wrap_ptr_(Args&&... args) {
    static_assert(!std::is_reference_v<T>, "T should not be a reference");

    T temp(std::forward<Args>(args)...);
    return std::make_unique<AnyInputWrapper<T>>(std::move(temp));
}

template<typename T, typename... Args>
AnyInput make_AnyInput(Args&&... args) {
    AnyInput a;
    a.emplace<T>(std::forward<Args>(args)...);
    return a;
};
} // namespace pluginplay::detail_
