#pragma once
#include "pluginplay/hasher.hpp"
#include "pluginplay/types.hpp"
#include "pluginplay/utility.hpp"
#include <any>
#include <memory>
#include <parallelzone/archive_wrapper.hpp>
#include <utilities/printing/print_stl.hpp>

namespace pluginplay::detail_ {
/// Forward declare class that will implement the API
template<typename T>
class AnyResultWrapper;

/// This should be moved to the utilities repo
template<typename T>
struct IsReferenceWrapper : std::false_type {};

template<typename T>
struct IsReferenceWrapper<std::reference_wrapper<T>> : std::true_type {};

template<typename T>
static constexpr bool is_reference_wrapper_v = IsReferenceWrapper<T>::value;

/**
 * @brief Defines the API for interacting with the type-erased value.
 *
 * This class is meant for use with the AnyResult class and can be thought
 * of as a PIMPL for that class. This actual implementation works by holding
 * the value in an std::any instance (thereby differing casting and type
 * checks to it) and leveraging as much of that API as possible. Additional
 * member functions that are part of the AnyResult API, but not std::any's API,
 * are then implemented by a derived class, which knows the type to cast the
 * std::any to.
 */
class AnyResultWrapperBase {
protected:
    /// My type
    using my_type = AnyResultWrapperBase;

    /// type for determining if @p T inherits from us
    template<typename T>
    using is_wrapper = std::is_base_of<my_type, T>;

    /// does @p T does not inherit from us?
    template<typename T>
    static constexpr bool is_not_wrapper_v = std::negation_v<is_wrapper<T>>;

    /// type for enabling a function if @p T does not stem from us
    template<typename T>
    using enable_if_not_wrapper_t = std::enable_if_t<is_not_wrapper_v<T>, int>;

public:
    /// Type of a unique_ptr to the wrapper
    using wrapper_ptr = std::unique_ptr<AnyResultWrapperBase>;

    /// Type of the RTTI of the wrapped instance
    using rtti_type = const std::type_info&;

    /// Type of the functions that take a Deserializer instance, deserialize
    /// from it an object of a specific type, and return, via a
    /// AnyResultWrapperBase pointer, the deserialized object wrapped in a
    /// AnyResultWrapper instance.
    using fxn_type = std::function<wrapper_ptr(parallelzone::Deserializer&)>;

    /** @brief Creates an AnyResultWrapper by forwarding the provided value.
     *
     *  @note This ctor only participates in overload resolution if @p T is
     * not AnyResultWrapperBase or any instanitation of
     * AnyResultWrapper.
     *
     *  @tparam T The type of the value we are wrapping.
     *
     *  @param[in] value The value we are wrapping.
     *
     *  @throw ??? throws if forwarding @p value to std::any's ctor throws.
     *             Same throw guarantee.
     */
    template<typename T, enable_if_not_wrapper_t<T> = 0>
    explicit AnyResultWrapperBase(T&& value) :
      m_value_(std::forward<T>(value)) {}

    /** @brief Cleans up an AnyResultBase_ instance.
     *
     *  Since AnyResultBase_ instances have no state this is a null
     * operation. However, it is important to mark this function as virtual
     * since we will literally always be passing the derived class around by
     * its base type.
     *
     *  @throws None. No throw guarantee.
     */
    virtual ~AnyResultWrapperBase() = default;

    /**
     *  @brief Makes a polymorphic copy of the wrapper.
     *
     *  This member calls clone_() in order to get a polymorphic deep copy
     * of the wrapper.
     *
     *  @return A deep copy, on the heap, of the wrapper.
     *
     *  @throw ??? If the copy constructor of the wrapped instance throws.
     * Same guarantee as the wrapped instance's copy ctor.
     */
    wrapper_ptr clone() const { return clone_(); }

    /** @brief Provides the RTTI of the wrapped class.
     *
     * This member calls type_ to get the RTTI of the wrapped instance.
     *
     * @return The RTTI of the wrapped class.
     *
     * @throw None No throw guarantee.
     */
    rtti_type type() const noexcept { return type_(); };

    /** @brief Determines if the wrapped instance is convertible to the
     *         provided type.
     *
     * @tparam T The type to try casting to.
     * @return True if the wrapped instance is convertible to type @p T and
     *         false otherwise.
     *
     * @throw None no throw guarantee.
     */
    template<typename T>
    bool is_convertible() const noexcept;

    /**
     *  @brief Allows the AnyResultBase_ instance to be hashed.
     *
     *  This function simply delegates to the protected hash_ member
     *  function (following the suggestion on BPHash's website).
     *
     *  @param[in, out] h A Hasher instance to use for the hashing.
     *
     *  @par Complexity:
     *  Same as the complexity of hashing the wrapped type.
     *
     *  @par Data Races:
     *  The state of the current instance will be accessed and data races
     *  may
     *  result if it is concurrently modified.
     *
     *  @throws ??? if the wrapped instance's hash function throws.  Strong
     *  throw guarantee.
     */
    void hash(Hasher& h) const { hash_(h); }

    /** @brief Enables serialization of the AnyResultBase_ instance.
     *
     *  @param[in,out] s The Serializer object that wraps an input archive
     * object.
     *
     *  @throws std::runtime_error if this instance holds a type-erased
     * reference; this may arise if you try to serialize a ModuleInput
     * instance.
     */
    void serialize(parallelzone::Serializer& s) const { serialize_(s); }

    /** @brief Enables deserialization of the AnyResultBase_ instance.
     *
     *  @param[in] d The Deserializer object that wraps an output archive
     * object.
     *
     */
    static wrapper_ptr deserialize(parallelzone::Deserializer& d);

    /** @brief Returns the string representation of the object stored in the
     *         any.
     *
     * @return A string representation of the
     */
    std::string str() const { return str_(); }

    /** @brief Used to determine if the type-erased value of this instance
     * is the same as that of another instance.
     *
     *  This function ultimately calls are_equal_ to determine if the two
     *  instances are equal. Equality of the wrapped values is determined by
     *  the result of calling operator== on the wrapped value. If the
     * wrapped instances are different types, as determined by their RTTI,
     * then they considered not equal.
     *
     *  @param[in] rhs the instance to compare to.
     *
     *  @return true if the wrapped instances compare equivalent and false
     *          otherwise.
     *
     *  @throw none No throw guarantee.
     */
    bool operator==(const AnyResultWrapperBase& rhs) const noexcept;

    /** @brief Used to determine if the type-erased value of this instance
     * is different from that of another instance.
     *
     *  This function ultimately negates a call to are_equal_ in order to
     *  determine if the two instances are different. Equality of the
     * wrapped values is determined by the result of calling operator== on
     * the wrapped value. If the wrapped instances are different types, as
     * determined by their RTTI, then they are considered different.
     *
     *  @param[in] rhs the instance to compare to.
     *
     *  @return false if the wrapped instances compare equivalent and true
     *          otherwise.
     *
     *  @throw none No throw guarantee.
     */
    bool operator!=(const AnyResultWrapperBase& rhs) const noexcept;

    /** @brief Casts the wrapped value back to a readonly type @p T
     *
     *  This function allows you to retrieve the wrapped value as long as
     * you request the value as a type it is implicitly convertible to. This
     *  particular overload additionally restricts you to deep copies of the
     *  wrapped value or read-only access.
     *
     *  @tparam T The type to retrieve the wrapped value as.
     * .
     *  @return The wrapped instance.
     *
     *  @throw std::bad_any_cast if the wrapped instance is not of type @p
     * T. Strong throw guarantee.
     */
    template<typename T>
    T cast() const {
        return std::any_cast<T>(m_value_);
    }

    /** @brief Casts the wrapped value back to a readonly type @p T
     *
     *  This function allows you to retrieve the wrapped value as long as
     * you request the value as a type it is implicitly convertible to. This
     *  particular overload allows you to get the wrapped value back in a
     * read write state.
     *
     *  @tparam T The type to retrieve the wrapped value as.
     * .
     *  @return The wrapped instance.
     *
     *  @throw std::bad_any_cast if the wrapped instance is not of type @p
     * T. Strong throw guarantee.
     */
    template<typename T>
    T cast() {
        return std::any_cast<T>(m_value_);
    }

protected:
    /** @brief Deep copies the type-erased value held in @p rhs.
     *
     *  This function is protected so it can be used by the derived class to
     *  implement clone_. Making it public opens us up to slicing, which we
     *  wish to avoid in the event that the derived class ever has state.
     *
     *  @param[in] rhs The instance to copy.
     *
     *  @throw ??? if the copying the value throws. Same guarantee.
     */
    AnyResultWrapperBase(const AnyResultWrapperBase&) = default;

    /** @brief Takes ownership of the type-erased value held in @p rhs.
     *
     *  This function is protected so it can be used by the derived class,
     * but is not public to avoid accidental slicing.
     *
     *  @param[in] rhs The instance to move from. After the move operation
     *                 @p rhs is in a valid, but otherwise undefined state.
     *
     *  @throw none No throw guarantee.
     */
    AnyResultWrapperBase(AnyResultWrapperBase&&) noexcept = default;

    /** @brief Sets the current state to a deep copy of @p rhs.
     *
     *  This function is protected so it can be used by the derived class,
     * but not public to avoid slicing.
     *
     *  @param[in] rhs The instance to copy.
     *
     *  @return the current instance containing a deep copy of @p rhs's
     * state.
     *
     *  @throw ??? if the copying the value throws. Same guarantee.
     */
    AnyResultWrapperBase& operator=(const AnyResultWrapperBase&) = default;

    /** @brief Replaces the current state with the type-erased value held
     *        in @p rhs.
     *
     *  This function is protected so it can be used by the derived class,
     * but is not public to avoid accidental slicing.
     *
     *  @param[in] rhs The instance to move from. After the move operation
     *                 @p rhs is in a valid, but otherwise undefined state.
     *
     *  @return The current instance set containing @p rhs's state.
     *
     *  @throw none No throw guarantee.
     */
    AnyResultWrapperBase& operator=(AnyResultWrapperBase&&) noexcept = default;

    /// Static map to store the hash_code of type_() as the keys and
    /// the functions that can return the deserialized object wrapped in a
    /// AnyResultWrapper instance as the values.
    inline static std::map<std::size_t, fxn_type> m_any_maker_;

private:
    /// To be implemented by derived class in order for copying to work
    virtual wrapper_ptr clone_() const = 0;

    /// To be implemented by derived class so we can retrieve the RTTI
    virtual rtti_type type_() const noexcept = 0;

    /// To be implemented by derived class so we can hash
    virtual void hash_(Hasher& h) const = 0;

    virtual void serialize_(parallelzone::Serializer& s) const = 0;

    /// To be implemented by derived class to make a string representation
    virtual std::string str_() const = 0;

    /// To be implemented by the derived class to define equality
    virtual bool are_equal_(const AnyResultWrapperBase& rhs) const noexcept = 0;

    /// The type-erased value
    std::any m_value_;
}; // class AnyResultWrapperBase

/** @brief The class responsible for holding the type-erased instance.
 *
 * The AnyResultWrapper class holds the wrapped instance for the
 * AnyResultWrapperBase class.  It also is responsible for implementing all
 * of the abstract methods
 *
 *
 * @tparam T The type of the instance to wrap. Must be copyable, hashable,
 * and have operator== defined.
 */
template<typename T>
class AnyResultWrapper : public AnyResultWrapperBase {
private:
    using base_type = AnyResultWrapperBase;

    template<typename U>
    using enable_if_not_any_t =
      typename base_type::template enable_if_not_wrapper_t<U>;

public:
    /**
     * @brief Creates a new AnyResultWrapper with the provided value
     *
     * @param[in] value The value we are wrapping.
     *
     * @throw ??? If @p T's ctor throws when @p value is forwarded to it.
     * Strong throw guarantee.
     */
    template<typename U, enable_if_not_wrapper_t<U> = 0>
    explicit AnyResultWrapper(U&& value) : base_type(std::forward<U>(value)) {}

protected:
    /** @brief Deep copies the type-erased value held in @p rhs.
     *
     *
     *  @param[in] rhs The instance to copy.
     *
     *  @throw ??? if the copying the value throws. Same guarantee.
     */
    AnyResultWrapper(const AnyResultWrapper<T>& rhs) = default;

    /** @brief Takes ownership of the type-erased value held in @p rhs.
     *
     *
     *  @param[in] rhs The instance to move from. After the move operation
     *                 @p rhs is in a valid, but otherwise undefined state.
     *
     *  @throw none No throw guarantee.
     */
    AnyResultWrapper(AnyResultWrapper<T>&&) noexcept = default;

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
    AnyResultWrapper<T>& operator=(const AnyResultWrapper<T>&) = default;

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
    AnyResultWrapper<T>& operator=(AnyResultWrapper<T>&&) noexcept = default;

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
     *  @return A newly allocated AnyResultBase_ instance.
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

    /** @brief Implements the equality comparison for the AnyResult
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
    bool are_equal_(const AnyResultWrapperBase& rhs) const noexcept override;

    /** @brief Implements hashing for the AnyResultBase_ class.
     *
     *  @param[in,out] h The hasher instance to use for the hashing.
     *
     *  @throws ??? if the wrapped instance's hash function throws.  Strong
     *              throw guarantee.
     */
    void hash_(Hasher& h) const override { h(value_()); }

    /** @brief Implements serialization for the AnyResultBase_ class.
     *
     *  @param[in,out] s The Serializer object that wraps an input archive
     * object.
     *
     *  @throws std::runtime_error if this instance holds a type-erased
     * reference; this may arise if you try to serialize a ModuleInput
     * instance.
     */
    void serialize_(parallelzone::Serializer& s) const override {
        // Reference wrappers show up for AnyResult instances that are wrapping
        // inputs. We don't need to serialize inputs

        constexpr bool is_ref_wrapper = is_reference_wrapper_v<T>;
        constexpr bool is_serializable =
          madness::is_serializable_v<parallelzone::BinaryOutputArchive, T>;
        if constexpr(is_ref_wrapper) {
            throw std::runtime_error("Are you trying to serialize an input?");
        } else if constexpr(is_serializable) {
            std::size_t idx = std::type_index(base_type::type()).hash_code();
            s(idx);
            if(!base_type::m_any_maker_.count(idx)) {
                typename base_type::fxn_type l =
                  [](parallelzone::Deserializer& d) {
                      std::decay_t<T> new_value;
                      d(new_value);
                      return std::make_unique<AnyResultWrapper>(
                        std::move(new_value));
                  };
                base_type::m_any_maker_[idx] = l;
            }
            s(value_());
        } else {
            throw std::runtime_error(
              "Unknown case for serialization of AnyResult. T is neither "
              "serializable, nor a "
              "reference wrapper. More work needed for serialization.");
        }
    }
};

//-------------------------------Implementations--------------------------------

template<typename T>
bool AnyResultWrapperBase::is_convertible() const noexcept {
    return std::any_cast<T>(&m_value_) != nullptr;
}

inline bool AnyResultWrapperBase::operator==(
  const AnyResultWrapperBase& rhs) const noexcept {
    return are_equal_(rhs);
}

inline bool AnyResultWrapperBase::operator!=(
  const AnyResultWrapperBase& rhs) const noexcept {
    return !((*this) == rhs);
}

inline typename AnyResultWrapperBase::wrapper_ptr
AnyResultWrapperBase::deserialize(parallelzone::Deserializer& d) {
    std::size_t idx;
    d(idx);
    return m_any_maker_.at(idx)(d);
}

template<typename U>
explicit AnyResultWrapper(U&& value)
  -> AnyResultWrapper<std::remove_reference_t<U>>;

template<typename T>
typename AnyResultWrapper<T>::wrapper_ptr AnyResultWrapper<T>::clone_() const {
    return std::make_unique<AnyResultWrapper<T>>(value_());
}

template<typename T>
std::string AnyResultWrapper<T>::str_() const {
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
bool AnyResultWrapper<T>::are_equal_(
  const AnyResultWrapperBase& rhs) const noexcept {
    try {
        return value_() == rhs.cast<const T&>();
    } catch(...) {
        // Means the cast failed, so @p rhs holds a different type
        return false;
    }
}

} // namespace pluginplay::detail_
