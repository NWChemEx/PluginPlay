#pragma once
#include "sde/detail_/sde_any_wrapper.hpp"
#include "sde/utility.hpp"
#include <iostream>
#include <memory>
#include <type_traits>

namespace sde::detail_ {

/** @brief The SDEAny class is capable of holding an instance of any type in a
 *         type-safe manner.
 *
 *  This class is used extensively through the SDE to avoid needing to know the
 *  types of inputs and results to a property type or module API. Under the hood
 *  it works by wrapping an std::any and casting that any to implement the
 *  additional functions which are part of the SDEAny's API, but not the
 *  std::any's API.
 */
class SDEAny {
private:
    /// Trait to see if @p T is an SDEAny
    template<typename T>
    using is_any = std::is_same<SDEAny, std::decay_t<T>>;

    /// True if @p T is an SDEAny
    template<typename T>
    static constexpr bool is_any_v = is_any<T>::value;

    /// True if @p T is not an SDEAny
    template<typename T>
    static constexpr bool not_an_any_v = std::negation_v<is_any<T>>;

    /// Enables a function if @p T is an SDEAny
    template<typename T>
    using enable_if_any_t = std::enable_if_t<is_any_v<T>, int>;

    /// Enables a function if @p T is not an SDEAny
    template<typename T>
    using enable_if_not_an_any_t = std::enable_if_t<not_an_any_v<T>, int>;

public:
    /// The type of rtti returned by the `type` function
    using rtti_type = typename SDEAnyWrapperBase::rtti_type;

    /// The type of the pointer holding the value
    using wrapper_ptr = typename SDEAnyWrapperBase::wrapper_ptr;

    /** @brief Makes an empty SDEAny instance.
     *
     *  The resulting SDEAny instance wraps no object.  An object can be added
     *  to this instance by calling the member function emplace, by assigning to
     *  this instance another SDEAny instance (containing a value), or by moving
     *  from another SDEAny instance containing a value.
     *
     *  @throw None. No throw guarantee.
     */
    SDEAny() noexcept = default;

    /** @brief Used to construct an SDEAny instance holding a particular value.
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
    template<typename T, enable_if_not_an_any_t<T> = 0>
    explicit SDEAny(T&& value);

    /**
     * @brief Makes a new SDEAny instance by deep copying the instance wrapped
     *        in another SDEAny instance.
     *
     * From the perspective of this class the copy ctor always makes a deep
     * copy. Whether it actually is a deep copy or not depends on the copy
     * constructor of the wrapped class.
     *
     * @param[in] rhs The SDEAny instance to copy.
     *
     * @throw std::bad_alloc if there is insufficient memory to copy the
     *                       instance stored in @p rhs.  Strong throw guarantee.
     * @throw ??? if the wrapped type's copy ctor throws.  Same guarantee as
     *            the wrapped type's copy ctor
     */
    SDEAny(const SDEAny& rhs) : m_ptr_(rhs.clone_()) {}

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
    SDEAny& operator=(const SDEAny& r) { return *this = std::move(SDEAny(r)); }

    /**
     * @brief Causes the current SDEAny instance to take ownership of another
     * instance.
     *
     * @param[in] rhs The SDEAny instance to take ownership of. After calling
     * this constructor @p rhs is in a valid, but undefined state.
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
    SDEAny(SDEAny&& rhs) noexcept = default;

    /**
     * @brief Sets the current instance to the state of another SDEAny instance.
     *
     * @param[in] rhs The SDEAny instance we are taking the state of.  After
     * this call @p rhs is in a valid, but otherwise undefined state.
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
    SDEAny& operator=(SDEAny&& rhs) noexcept = default;

    /** @brief Frees up the memory wrapped by the SDEAny instance.
     *
     *  This is a default dtor. After calling it all references to the
     *  underlying type-erased instance are no longer valid.
     *
     *  @throw None. No throw guarantee.
     */
    ~SDEAny() = default;

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
     * action.  Let's say you have an SDEAny instance and you want it to wrap a
     * double, this could be done like:
     *
     * @code
     * SDEAny my_any;
     * double& wrapped_value = my_any.emplace<double>(3.14);
     * @endcode
     *
     * For a more complicated type, like an std::vector<double> we could instead
     * do:
     *
     * @code
     * SDEAny my_any;
     * std:vector<double> vec1{1.1, 1.2, 1.3};
     *
     * // Makes a copy of vec1
     * auto& wrapped_ve1 = my_any.emplace<std::vector<double>>(vec1);
     *
     * // ...makes vec1, inside the SDEANy, without the copy
     * auto& wrapped_v2 = my_any.emplace<std::vector<double>>({1.1, 1.2, 1.3});
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
     *  SDEAny instances own the memory they are wrapping.  This member function
     *  can be used to release the memory before the SDEAny instance goes out
     *  of scope. Following normal RAII practices the SDEAny will release any
     *  held memory in its dtor meaning users need not call this function before
     *  letting an SDEAny instance go out of scope.
     *
     * @throw None. No throw guarantee.
     */
    void reset() noexcept { m_ptr_.reset(); }

    /** @brief Swaps the states of two SDEAny instances.
     *
     *  After a call to this function, the current instance will contain
     *  @p rhs's state and @p rhs will contain the current instance's state.
     *
     * @param[in,out] rhs The SDEAny instance to swap contents with. After this
     *                call @p rhs will contain the current instance's state.
     *
     * @throw None. No throw guarantee.
     *
     * @par Complexity constant.
     */
    void swap(SDEAny& rhs) noexcept { m_ptr_.swap(rhs.m_ptr_); }

    /** @brief Used to retrieve the value wrapped in the SDEAny.
     *
     *  The canonical way to retrieve the value from an SDEAny is to use
     *  SDEAnyCast. This function is the implementation for the cast and can be
     *  called directly if the user wants.
     *
     * @tparam T The type to cast the wrapped SDEAny instance to. Should include
     *         cv qualifiers and include reference/pointer designations if so
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

    /** @brief Used to retrieve the value wrapped in a read-only SDEAny.
     *
     *  The canonical way to retrieve the value from an SDEAny is to use
     *  SDEAnyCast. This function is the implementation for the cast and can be
     *  called directly if the user wants. This overload will only allow you to
     *  retrieve the value in a read-only state.
     *
     * @tparam T The type to cast the wrapped SDEAny instance to. Should include
     *         cv qualifiers and include reference/pointer designations if so
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

    /** @brief Returns true if the current SDEAny instance is presently wrapping
     *         a value.
     *
     *  An SDEAny can either be holding a type-erased value or not. This
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
    void hash(runtime::Hasher& h) const { h(m_ptr_); }

    /** @brief Creates a human-readable string representation of the wrapped
     *         instance.
     *
     *  This function ultimately works by printing "<empty SDEAny>" if the
     *  current instance does not contain a value, or the result of forwarding
     *  the wrapped value to `std::ostream::operator<<`. Overloads of this
     *  operator for STL containers are leveraged.
     *
     *  @throw std::bad_alloc if there is insufficient memory to create the
     *                        string. Strong throw guarantee.
     *  @throw ??? if std::ostream<< throws when provided the wrapped value.
     *             Same guarantee as std::ostream<<.
     */
    std::string str() const;

    /** @brief Compares two SDEAny instances for equality.
     *
     * Two SDEAny instances are equal if they both hold a value (or do not hold
     * a value) and, in the case that they both hold a value, the wrapped values
     * compare equal.
     *
     * @param[in] rhs The instance to compare to
     *
     * @return true if the SDEAny instances are equal and false otherwise.
     *
     * @throw None. All comparisons are no throw guarantee.
     */
    bool operator==(const SDEAny& rhs) const noexcept;

    /** @brief Compares two SDEAny instances to see if they are different.
     *
     * Two SDEAny instances are equal if they both hold a value (or do not hold
     * a value) and, in the case that they both hold a value, the wrapped values
     * compare equal.
     *
     * @param[in] rhs The instance to compare to
     *
     * @return false if the SDEAny instances are equal and true otherwise.
     *
     * @throw None. All comparisons are no throw guarantee.
     */
    bool operator!=(const SDEAny& rhs) const noexcept;

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
     * @tparam T The cv qualified type that this SDEAny instance will be
     *           wrapping.
     *
     * @tparam Args The types of the arguments that will be provided to @p T's
     *              ctor.
     *
     * @param[in] args The arguments to forward to @p T's ctor.
     *
     * @return A unique_ptr containing an SDEAnyWrapperBase instance which wraps
     *         the newly created @p T instance.
     *
     * @throws std::bad_alloc if there is insufficient memory to allocate a
     *                        new SDEWrapper<T> instance.  Strong throw
     *                        guarantee.
     * @throws ??? if @p T's ctor throws. Same guarantee as @p T's ctor.
     */
    template<typename T, typename... Args>
    static wrapper_ptr wrap_ptr_(Args&&... args);

    /// The object holding the actual value
    wrapper_ptr m_ptr_;
};

/** @brief Provides access to the value wrapped in an SDEAny instance.
 *
 * @relates SDEAny
 *
 * @tparam T The type of the value wrapped in the SDEAny instance.
 * @tparam U The type of the SDEAny instance itself. Should be at most
 *           cv-qualified SDEAny.
 *
 * @param[in] da_any The SDEAny instance to retrieve the value from.
 *
 * @return The value wrapped by @p da_any.
 *
 * @throw std::bad_any_cast if the value wrapped by @p da_any is not
 *                          convertible to type @p T or if @p da_any does not
 *                          contain a value.  Strong throw guarantee.
 *
 * @par Complexity: Constant.
 */
template<typename T, typename U>
T SDEAnyCast(U&& da_any) {
    return da_any.template cast<T>();
}

/** @brief Makes an SDEAny instance by forwarding the arguments to the wrapped
 *  instance's constructor.
 *
 *  @relates SDEAny
 *
 *  This is a convenience function for directly populating an SDEAny instance so
 *  that the user does not have to first construct a temporary and then forward
 *  that temporary to an SDEAny instance.
 *
 *  @tparam T The type of the object that the SDEAny should wrap.
 *  @tparam Args The types of the arguments that are being forwarded.
 *
 *  @param[in] args The values to forward to @p T's ctor.
 *
 *  @return An SDEAny instance containing a newly created wrapped @p T instance.
 *
 *  @par Data Races:
 *  The values of @p args are forwarded to T's ctor and data races may occur if
 *  the values of args are concurrently modified.
 *
 *  @throw std::bad_alloc if there is insufficient memory for the SDEAny to
 *                        create the wrapper.  Strong throw guarantee.
 *  @throw ??? If @p T's constructor throws.  Same guarantee as T's constructor.
 *
 *  @par Complexity:
 *  Same as T's ctor taking @p args.
 *
 */
template<typename T, typename... Args>
SDEAny make_SDEAny(Args&&... args);

//--------------------------Implementations-------------------------------------

template<typename T, typename SDEAny::enable_if_not_an_any_t<T>>
SDEAny::SDEAny(T&& value) :
  m_ptr_(wrap_ptr_<std::conditional_t<std::is_reference_v<T>,
                                      std::remove_reference_t<T>, T>>(
    std::forward<T>(value))) {}

template<typename T, typename... Args>
T& SDEAny::emplace(Args&&... args) {
    wrap_ptr_<T>(std::forward<Args>(args)...).swap(m_ptr_);
    return m_ptr_->cast<T&>();
};

template<typename T>
T SDEAny::cast() {
    if(!has_value()) throw std::bad_any_cast();
    return m_ptr_->cast<T>();
}

template<typename T>
T SDEAny::cast() const {
    if(!has_value()) throw std::bad_any_cast();
    // Not sure if it's a compiler bug or I'm missing something, but for some
    // reason just doing ->cast<T>() calls the non-const version
    const auto* p = m_ptr_.get();
    return p->cast<T>();
}

inline typename SDEAny::rtti_type SDEAny::type() const noexcept {
    return has_value() ? m_ptr_->type() : typeid(void);
}

inline std::string SDEAny::str() const {
    if(has_value()) return m_ptr_->str();
    return "<empty SDEAny>";
}

inline bool SDEAny::operator==(const SDEAny& rhs) const noexcept {
    if(has_value() != rhs.has_value()) return false;
    if(!has_value()) return true;
    return (*m_ptr_) == (*rhs.m_ptr_);
}

inline bool SDEAny::operator!=(const SDEAny& rhs) const noexcept {
    return !((*this) == rhs);
}

inline typename SDEAny::wrapper_ptr SDEAny::clone_() const {
    return has_value() ? m_ptr_->clone() : wrapper_ptr{};
}

template<typename T, typename... Args>
typename SDEAny::wrapper_ptr SDEAny::wrap_ptr_(Args&&... args) {
    static_assert(!std::is_reference_v<T>, "T should not be a reference");

    T temp(std::forward<Args>(args)...);
    return std::make_unique<SDEAnyWrapper<T>>(std::move(temp));
}

template<typename T, typename... Args>
SDEAny make_SDEAny(Args&&... args) {
    SDEAny a;
    a.emplace<T>(std::forward<Args>(args)...);
    return a;
};
} // namespace sde::detail_
