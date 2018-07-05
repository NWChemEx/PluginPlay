#pragma once
#include "SDE/Memoization.hpp"
#include "SDE/Pythonization.hpp"
#include <memory>
#include <type_traits>

#include <SDE/Serialization.hpp>

namespace SDE {

/// Namespace for classes not meant to be part of the SDE's public API
namespace detail_ {

/** @brief The SDEAny class is capable of holding an instance of any type in a
 *  type-safe manner.
 *
 *  The primary motivation for this class is sometimes you just want to store a
 *  bunch of objects, of different types, in the same container.  Since in C++
 *  types do not all inherit from some common type (like Python's Object class
 *  for example) the SDEAny class provides a mock common base class.  Ultimately
 *  this class works by type-erasure.
 *
 *  To use this class:
 *  @code
 *  //To put value into the SDEAny instance
 *  double value = 2.0;
 *  auto wrapped_value = make_SDEAny(value);
 *
 *  //To get the value back
 *  double retrieved_value = SDEAnyCast<double>(wrapped_value);
 *  @endcode
 *
 *
 *  @note Interestingly the universal reference constructor takes precedence
 *  over the copy constructor, hence the meta-template programming.
 *
 */
class SDEAny {
private:
    /** @brief Class for determining whether or not a type is derived from
     *  SDEAny.
     *
     *  If @p T is derived from SDEAny (or is an SDEAny), then the resulting
     *  typedef, is_related, will contain a bool member variable called
     *  "value" set to true, otherwise "value" is false.
     *
     *  @tparam T The type to check for inheritance.
     */
    template<typename T>
    using is_related =
      std::is_base_of<SDEAny, typename std::remove_reference<T>::type>;

    /** @brief Class for disabling a function via SFINAE if the input is derived
     *  from SDEAny.
     *
     *  If @p T is derived from SDEAny (or is an SDEAny), then the resulting
     *  typedef, disable_if_related, will contain a member typedef "type"
     *  set that is an alias for void, otherwise "type" will not exist.
     *
     *  @tparam T The type to check for inheritance.
     */
    template<typename T>
    using disable_if_related =
      typename std::enable_if<!is_related<T>::value>::type;

public:
    /** @brief Makes an empty SDEAny instance.
     *
     *  The resulting SDEAny instance wraps no object.  An object can be added
     * to it by calling the member function emplace.
     *
     *  @par Complexity:
     *  Constant.
     *
     *  @par Data Races:
     *  None.
     *
     *  @throw None. No throw guarantee.
     */
    SDEAny() = default;

    /**
     * @brief Makes a new SDEAny instance by copying an already existing SDEAny
     * instance (and its wrapped instance).
     *
     * This function will invoke the copy constructor of the wrapped class. Thus
     * whether this function results in a deep copy or a shallow copy depends on
     * the semantics of the wrapped type.
     *
     * @param[in] rhs The SDEAny instance to copy.
     *
     * @par Complexity:
     * Same as the copy constructor of the wrapped type.
     *
     * @par Data Races:
     * The state of @p rhs will be accessed. Data races may result if @p rhs is
     * modified during the copy.
     *
     * @throw std::bad_alloc if there is insufficient memory to copy the
     * instance stored in @p rhs.  Strong throw guarantee.
     * @throw ??? if the wrapped type's constructor throws.  Strong throw
     * guarantee.
     */
    SDEAny(const SDEAny& rhs) {
        if(rhs.ptr_) ptr_ = std::move(rhs.ptr_->clone());
    }

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
    SDEAny& operator=(const SDEAny& rhs) {
        if(this != &rhs && rhs.ptr_)
            ptr_ = std::move(rhs.ptr_->clone());
        else if(!rhs.ptr_)
            ptr_.release();
        return *this;
    }

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
    SDEAny(SDEAny&& rhs) = default;

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
     *  @par Complexity:
     *  Constant.
     *
     *  @par Data Races:
     *  The state of the current instance is modified and attempts to
     *  concurrently access it may result in a data race.
     *
     *  @throw None. No throw guarantee.
     */
    ~SDEAny() = default;

    /**
     * @brief Used to construct an SDEAny instance holding a particular value.
     *
     * The value held by the resulting instance contains a copy of @p value's
     * state if @p value is an lvalue.  If @p value is a an rvalue, then the
     * resulting instance contains @p value's state and @p value is in a valid,
     * but otherwise undefined state.
     *
     * @tparam T The type of instance to wrap.  Must meet the concept of
     * copyable.
     * @tparam X A dummy type used to prevent this function from
     * participating in resolution if T is related to SDEAny (in which case
     * we're really trying to copy/move the other SDEAny instance).
     *
     * @param[in] value The instance to wrap.
     *
     * @par Complexity:
     * For copying, same as the copy constructor of @p T.  For moving constant.
     *
     * @par Data Races:
     * The content of @p value will be accessed (for copying) or modified (for
     * moving).  Either way, data races may result if @p value is concurrently
     * accessed.
     *
     * @throws std::bad_alloc if memory allocation fails.  Strong throw
     * guarantee.
     * @throws ??? If the copy/move constructor of @p T throws. Strong throw
     * guarantee.
     */
    template<typename T, typename X = disable_if_related<T>>
    explicit SDEAny(T&& value) :
      ptr_(std::move(wrap_ptr<std::decay_t<T>>(std::forward<T>(value)))) {}

    /**
     *  @brief Allows the SDEAny instance to be hashed.
     *
     *  @param[in, out] h A Hasher instance to use for the hashing.
     *
     *  @par Complexity:
     *  Same as the complexity of hashing the wrapped type.
     *
     *  @par Data Races:
     *  The state of the current instance will be accessed and data races may
     *  result if it is concurrently modified.
     *
     *  @throws ??? if the wrapped instance's hash function throws.  Strong
     *  throw guarantee.
     */
    void hash(Hasher& h) const { h(ptr_); }

    /**
     *  @brief Allows the SDEAny instance to be saved to a Cereal output
     *  archive.
     *
     *  @param[in] ar A Cereal output archive instance in which the SDEAny
     *  instance will be saved.
     *
     *  @param[in] key A string label which is used to index the SDEAny
     *  entry in the Cereal archive. key is typically a unique hash value.
     */
    template <class OutputArchive>
    std::enable_if_t<
            std::is_same_v<OutputArchive, cereal::HDF5OutputArchive> ||
            std::is_same_v<OutputArchive, cereal::JSONOutputArchive> ||
            std::is_same_v<OutputArchive, cereal::XMLOutputArchive> >
    save(OutputArchive& ar, const std::string& key = "archive_entry") const
    {
        ar(cereal::make_nvp(key, ptr_));
    }

    /**
     *  @brief Loads the data from an SDEAny instance stored in a Cereal input
     *  archive, and reconstructs the SDEAny.
     *
     *  @param[in] ar A Cereal input archive instance in which the SDEAny
     *  data is saved.
     *
     *  @param[in] key A string label corresponding to the index of the SDEAny
     *  data stored in the Cereal archive. key is typically a unique hash value.
     */
    template <class InputArchive>
    std::enable_if_t<
            std::is_same_v<InputArchive, cereal::HDF5InputArchive> ||
            std::is_same_v<InputArchive, cereal::JSONInputArchive> ||
            std::is_same_v<InputArchive, cereal::XMLInputArchive> >
    load(InputArchive& ar, const std::string& key)
    {
        ar(cereal::make_nvp(key, ptr_));
    }

    /**
     * @brief Returns the type of the wrapped instance.
     *
     * This function returns the RTTI of the wrapped instance.  It should be
     * noted that the result is compiler specific and should be used with
     * caution.  If the current instance is not wrapping anything the result
     * will be typeid(nullptr).
     *
     * @par Complexity:
     * Constant.
     *
     * @par Data Races:
     * The state of the current instance is accessed and data races may occur if
     * the current instance is concurrently modified.
     *
     * @return The RTTI of the wrapped type.
     * @throw None. No throw guarantee.
     */
    const std::type_info& type() const noexcept {
        return ptr_ ? ptr_->type() : typeid(nullptr);
    }

    /**
     * @brief Releases the wrapped memory associated with the present SDEAny
     * instance.
     *
     * SDEAny instances own the memory they are wrapping.  This member function
     * can be used to release the memory before the SDEAny instance goes out
     * of scope.
     *
     * @par Complexity:
     * Constant.
     *
     * @par Data Races:
     * The contents of the current instance are modified and data races may
     * occur if the instance is concurrently accessed.
     *
     * @throw None. No throw guarantee.
     */
    void reset() noexcept { ptr_.reset(); }

    /**
     * @brief Swaps the states of two SDEAny instances.
     *
     * @par Complexity:
     * Constant.
     *
     * @par Data Races.
     * The contents of both this and @p rhs are modified.  Data races may ensure
     * if attempts are made to access or modify either instance concurrently.
     *
     * @param[in,out] rhs The SDEAny instance to swap contents with. After this
     * call @p rhs will contain the current instance's state.
     *
     * @throw None. No throw guarantee.
     */
    void swap(SDEAny& rhs) noexcept { ptr_.swap(rhs.ptr_); }

    /**
     * @brief Returns true if the current SDEAny instance is presently wrapping
     * a value.
     *
     * @par Complexity:
     * Constant.
     *
     * @par Data Races:
     * The contents of the current instance are accessed and data races may
     * ensue if the instance is concurrently modified.
     *
     * @return True if the current instance has allocated memory and False
     * otherwise.
     *
     * @throw None. No throw guarantee.
     */
    bool has_value() const noexcept { return static_cast<bool>(ptr_); }

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
    std::decay_t<T>& emplace(Args&&... args) {
        using no_cv = std::decay_t<T>;
        ptr_        = wrap_ptr<no_cv>(std::forward<Args>(args)...);
        return cast<no_cv>();
    };

    /**
     * @brief Facilitates using SDEAny in classes exposed to Python.
     *
     * SDEAny itself is not exposed to Python; however, classes like Parameters
     * are exposed to Python and contain SDEAny instances.  This function will
     * transform the type-erased object into an object that can be accessed from
     * Python without needing to know the wrapped instance's type.
     *
     * @return The object wrapped in this instance as a pybind11::object.
     * @throws pybind11::cast_error if the wrapped type can not be converted to
     *         a Python type.  Strong throw guarantee.
     * @throws std::runtime_error if this function is called and Python bindings
     *         were not enabled.  Strong throw guarantee.
     *
     */
    pyobject pythonize() const { return ptr_->pythonize(); }

private:
    /// Allows SDEAnyCast to return the wrapped value
    template<typename T>
    friend T& SDEAnyCast(SDEAny&);

    /**
     * @brief Class to hold the type-erased instance.
     *
     * This class will ultimately serve as the opaque API to the wrapped
     * instance.  If one has read the discussion @ref te_solution then it's
     * worth noting that SDEAnyBase is the realization of the TEBase class.
     *
     * Note that this class is abstract and can not be instantiated.
     */
    struct SDEAnyBase_ {
        /**
         *  @brief Cleans up an SDEAnyBase_ instance.
         *
         *  Since SDEAnyBase_ instances have no state this is a null operation.
         *  However, it is important to mark this function as virtual to ensure
         *  that we don't end up with a memory leak caused by the derived class
         *  not deleting its data when we pass that class around via the base
         *  class (which is literally all the time).
         *
         *  @par Complexity:
         *  Constant.
         *
         *  @par Data Races:
         *  None.
         *
         *  @throws None. No throw guarantee.
         */
        virtual ~SDEAnyBase_() = default;

        /**
         *  @brief Makes a polymorphic copy of the wrapper.
         *
         *  This abstract method should be implemented in the derived class by
         *  creating a new SDEAnyBase_ on the heap by calling the copy ctor of
         *  the derived class.  The resulting copy should be returned inside a
         *  unique_ptr
         *
         *  @return A newly allocated copy of the wrapped instance.
         *
         */
        virtual std::unique_ptr<SDEAnyBase_> clone() = 0;

        /**
         * @brief Provides the RTTI of the wrapped class.
         *
         * This member should be implemented so that it returns the result of
         * calling `typeid()` on the wrapped type.
         *
         * @return The RTTI of the wrapped class.
         */
        virtual const std::type_info& type() const noexcept = 0;

        /**
         *  @brief Allows the SDEAnyBase_ instance to be hashed.
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

        /// Public API for virtual python function
        pyobject pythonize() { return pythonize_(); }

    protected:
        /// The function for hashing, to be implemented by the derived class
        virtual void hash_(Hasher& h) const = 0;

        /// Function for converting to read-only python object,
        virtual pyobject pythonize_() const = 0;
    };

    /**
     * @brief The class in the SDEAny hierarchy responsible for holding the
     * wrapped instance.
     *
     * The SDEAnyWrapper_ class holds the wrapped instance for the SDEAnyBase_
     * class.  It also is responsible for implementing the abstract methods in
     * the latter.
     *
     * @tparam T The type of the instance to wrap.  Must satisfy the concept of
     * copyable.
     */
    template<typename T>
    struct SDEAnyWrapper_ : SDEAnyBase_ {
        /**
         * @brief Creates a new SDEAnyWrapper_ by copying a value.
         *
         * @param[in] value_in a copy of the value.
         *
         * @par Complexity:
         * Constant.
         *
         * @par Data Race:
         * None.  Data is copied in and the copied value is wrapped.
         *
         * @throw ??? If T's move constructor throws.  Strong throw guarantee.
         *
         */
        SDEAnyWrapper_(const T& value_in) : value(value_in) {}

        /**
         * @brief Creates a new SDEAnyWrapper_ by taking ownership of an
         * already existing @p T instance.
         *
         * @param[in] value_in the instance whose state we are stealing.  After
         * this call @p value_in will be in a valid, but otherwise undefined
         * state.
         *
         * @par Complexity:
         * Same as T's move constructor.
         *
         * @par Data Race:
         * @p value_in is modified and data races may occur if it is
         * concurrently accessed.
         *
         * @throw ??? If T's move constructor throws.  Strong throw guarantee.
         *
         */
        SDEAnyWrapper_(T&& value_in) : value(std::move(value_in)) {}

        /// The actual wrapped value
        T value;

        /** @brief Polymorphic copy function that returns an SDEAnyBase_
         * allocated on the heap by copying the current instance.
         *
         *  @return A newly allocated SDEAnyBase_ instance.
         *
         *  @par Complexity:
         *  Same as T's copy ctor.
         *
         *  @par Data Races:
         *  The value wrapped by the current instance is accessed and data races
         *  may occur if it is concurrently modified.
         *
         *  @throw std::bad_alloc if there is insufficient memory to copy.
         *  Strong throw guarantee.
         *  @throw ??? If @p T's copy constructor throws.  Same guarantee as
         *  T's copy constructor.
         */
        std::unique_ptr<SDEAnyBase_> clone() override {
            return std::move(std::make_unique<SDEAnyWrapper_<T>>(*this));
        }

        /**
         * @brief Returns the RTTI of the wrapped instance.
         *
         * @return The RTTI of the wrapped instance.
         *
         * @par Complexity:
         * Constant.
         *
         * @par DataRaces:
         * None.
         *
         * @throws None. No throw guarantee.
         */
        const std::type_info& type() const noexcept override {
            return typeid(T);
        }

        /**
         *  @brief Implements hashing for the SDEAnyBase_ class.
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
        template<class Archive>
                void serialize(Archive& ar)
        {
            // Bind derive and base class relationship to support
            // polymorphic serialization
            cereal::base_class_detail::RegisterPolymorphicBaseClass
            <SDEAnyBase_, SDEAnyWrapper_<T> >::bind();

            ar(value);
        }

    protected:
        /**
         *  @brief Implements hashing for the SDEAnyBase_ class.
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
        virtual void hash_(Hasher& h) const override { h(value); }

        /**
         * @brief Allows the wrapped object to be returned as the opaque Python
         * base class.  The returned instance is read-only.
         *
         * @return The object wrapped in an opaque Python base class
         * @throws pybind11::cast_error if the cast fails.  Strong throw
         * guarantee.
         * @throws std::runtime_error if Python bindings are not enabled.
         * Strong throw guarantee.
         */
        virtual pyobject pythonize_() const override {
            return pycast<T>::cast(value);
        }
    };

    /**
     * @brief Code factorization for the internal process of wrapping an
     * instance.
     *
     * The actual process of making the wrapped instance requires us to:
     * 1. Remove decorators (e.g. const) to get at the fundamental type.
     * 2. Ensure the type is copyable.
     * 3. Forwarding the input arguments to @p T's ctor.
     *
     * This function wraps that procedure.
     *
     * @tparam T The fully decorated type of the object we are wrapping.  Must
     * be copyable.
     *
     * @tparam Args The types of the arguments that will be provided to @p T's
     *         ctor.
     *
     * @param args The actual arguments to @p T's ctor.
     * @return A new SDEAnyBase_ instance allocated on the heap.
     * @throws std::bad_alloc if there is insufficient memory to allocate a
     *         new SDEWrapper<T> instance.  Strong throw guarantee.
     * @throws ??? if T's ctor throws. Strong throw guarantee.
     */
    template<typename T, typename... Args>
    std::unique_ptr<SDEAnyBase_> wrap_ptr(Args&&... args) const {
        using no_cv = std::decay_t<T>;
        static_assert(std::is_copy_constructible<no_cv>::value,
                      "Only copy constructable objects may be assigned to "
                      "SDEAny instances");
        using result_t = SDEAnyWrapper_<no_cv>;
        return std::move(
          std::make_unique<result_t>(std::forward<Args>(args)...));
    }

    /**
     * @brief The actual downcast.
     *
     * @tparam T The type to cast to.
     * @return The wrapped value.
     */
    template<typename T>
    T& cast() {
        if(typeid(T) != ptr_->type()) throw std::bad_cast{};
        return static_cast<SDEAny::SDEAnyWrapper_<T>&>(*ptr_).value;
    }

    /// The actual type-erased value
    std::unique_ptr<SDEAnyBase_> ptr_;
};

/**
 * @brief Provides access to the value wrapped in an SDEAny instance.
 * @relates SDEAny
 * @tparam T The type to cast the SDEAny instance to.
 * @param[in] wrapped_value An any instance containing a value.
 * @return The value wrapped by @p wrapped_value.
 * @throw std::bad_cast if the value wrapped by @p wrapped_value is not
 * convertible to type @p T.  Strong throw guarantee.
 * @par Complexity:
 * Constant.
 * @par Data Races:
 * The state of @p wrapped_value is accessed and data races may occur if
 * @p wrapped_value is concurrently modified.
 */
template<typename T>
T& SDEAnyCast(SDEAny& wrapped_value) {
    return wrapped_value.cast<T>();
}

/**
 * @brief Provides access to the value wrapped in a read-only SDEAny instance.
 * @relates SDEAny
 *
 * Rather than writing two versions of SDEAny::cast (which are the same aside
 * from their const-ness), we instead opt for a const_cast.  This is justified
 * because the returned value is provided to the user as const reference.  Thus
 * the user still interacts with the SDEAny instance in a read-only fashion.
 *
 * @tparam T The type to cast the SDEAny instance to.
 * @param wrapped_value An any instance containing a value.
 * @return The value, in a read-only state, that is wrapped by @p wrapped_value.
 * @throw std::bad_cast if the value wrapped by @p wrapped_value is not
 * convertible to type @p T.  Strong throw guarantee.
 * @par Complexity:
 * Constant.
 * @par Data Races:
 * The state of @p wrapped_value is accessed and data races may occur if
 * @p wrapped_value is concurrently modified.
 */
template<typename T>
const T& SDEAnyCast(const SDEAny& wrapped_value) {
    return SDEAnyCast<T>(const_cast<SDEAny&>(wrapped_value));
}

/** @brief Makes an SDEAny instance by forwarding the arguments to the wrapped
 *  instance's constructor.
 *
 *  This is a convenience function for directly populating an SDEAny instance so
 *  that the user does not have to first construct a temporary and then forward
 *  that temporary to an SDEAny instance.
 *
 *  @relates SDEAny
 *
 *  @param args The arguments to forward to @p T's constructor.
 *  @tparam T The type of the object we are going to wrap.
 *  @tparam Args The types of the arguments that are being forwarded.
 *  @return An any instance constructed with the current arguments.
 *
 *  @par Complexity:
 *  Same as T's ctor taking @p args.
 *
 *  @par Data Races:
 *  The values of @p args are forwarded to T's ctor and data races may occur if
 *  the values of args are concurrently modified.
 *
 *  @throw std::bad_alloc if there is insufficient memory for the resulting
 *  instance.  Strong throw guarantee.
 *  @throw ??? If @p T's constructor throws.  Same guarantee as T's constructor.
 *
 */
template<typename T, typename... Args>
SDEAny make_SDEAny(Args&&... args) {
    return SDEAny(std::move(T(std::forward<Args>(args)...)));
};

} // namespace detail_
} // namespace SDE


/** @brief Provides Cereal with an appropriate method to load SDEAny data
 * from an input archive and reconstruct the SDEAny.
 */
template<typename T> struct
cereal::LoadAndConstruct<SDE::detail_::SDEAny::SDEAnyWrapper_<T> >{
    template <class Archive>
    static void load_and_construct(Archive& ar,
				   cereal::construct<SDE::detail_::SDEAny::SDEAnyWrapper_<T> >& construct){
        T value;
        ar(value);
        construct(value);
    }
};

//Register derived class types with Cereal to support polymorphic serialization
CEREAL_REGISTER_TYPE(SDE::detail_::SDEAny::SDEAnyWrapper_<double>);
CEREAL_REGISTER_TYPE(SDE::detail_::SDEAny::SDEAnyWrapper_<int>);
CEREAL_REGISTER_TYPE(SDE::detail_::SDEAny::SDEAnyWrapper_<std::string>);
CEREAL_REGISTER_TYPE(SDE::detail_::SDEAny::SDEAnyWrapper_<std::vector<double>>)/

