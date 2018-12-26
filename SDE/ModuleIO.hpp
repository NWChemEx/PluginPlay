#pragma once
#include "SDE/detail_/SDEAny.hpp"
#include "SDE/detail_/Memoization.hpp"
#include <Utilities/Containers/CaseInsensitiveMap.hpp>
#include <functional>



namespace SDE {
namespace detail_ {

/** @brief Code factorization for members/state of ModuleInput and ModuleOutput
 *  classes.
 *
 *  Together the ModuleIO, ModuleInput, and ModuleOutput classes form what we
 *  call the "module input/output classes" or "module I/O classes" for short.
 *  The present class, ModuleIO, is a base class containing functionality common
 *  to the other module I/O classes. ModuleIO is not meant to be used directly,
 *  rather classes wanting to interact with a module's input and output should
 *  work directly with ModuleInput/ModuleOutput depending on which aspect they
 *  intend to interact with.
 *
 *  The module I/O classes each hold an input or output value and some metadata
 *  associated with that value. Since there is a rather large number of metadata
 *  parameters associated with any given value we have chosen to adopt the
 *  builder pattern for initialization (
 *  [wikipedia](https://en.wikipedia.org/wiki/Builder_pattern)).  Basically,
 *  what this amounts to is when a module developer declares, say an input, they
 *  do so like:
 *
 *  ```
 *  add_input<int>("input name");
 *  ```
 *
 *  This makes an input parameter accessible with the key `"input name"` that is
 *  of type `int`. The `add_input` function returns a builder object of type
 *  ModuleInput which can be used to initialize the input in a clean manner. For
 *  example extending the above code so that we declare our parameter to be
 *  transparent and have a default value of 4 would be done like:
 *
 *  ```
 *  add_input<int>("input name").transparent().default_value(4);
 *  ```
 *
 *  Here the order of the calls to `transparent` and `default_value` are
 *  irrelevant. Aside from the slightly unique mechanism for initializing the
 *  class the rest of the module I/O classes is pretty straight forward being
 *  made up primarily of getters/setters.
 *
 */
class ModuleIO {
private:
    ///Type of the type-erased holder class
    using any_type = SDEAny;

    ///Type of a callback for checking a value via the type-erased holder
    using check_fxn = std::function<bool(const any_type&)>;

    ///Type w/o the reference
    template<typename T> using no_ref = std::remove_reference_t<T>;

    ///Type of the class wrapping a reference
    template<typename T> using ref_wrapper = std::reference_wrapper<no_ref<T>>;

public:
    ///Type of a value's input
    using description_type = std::string;

    ///Type user-provided callbacks should satisfy.
    template<typename T>
    using value_checker = std::function<bool(const T&)>;

    ///Basic dtor, virtual because we derive from it, but otherwise not special
    virtual ~ModuleIO() = default;

    /** @brief Returns the RTTI for values that can be wrapped by this class.
     *
     * If this instance is not holding a value the returned value is
     * `typeid(std::nullptr)` unless `set_type<T>` has been called, in which
     * case the returned value is `typeid(T)`.
     *
     * @return The RTTI type of the currently held value
     * @throw None No throw guarantee.
     */
    const auto& type() const noexcept { return type_(); }

    //@{
    /** @name Value accessors
     *
     * 1) Read-only value access
     * 2) Read/write value access
     *
     * @tparam T The type the wrapped value should be returned as. This type
     *         should be fully qualified, *e.g.*, if the value passed is a
     *         constant reference to an integer `T=const int&` not `T=int`.
     * @return The value contained within this class.
     * @throw std::bad_cast if the wrapped value cannot be converted to type
     *        @p T. Strong throw guarantee.
     */
    template<typename T>
    T value() const {
        if(is_ref_)
            return SDEAnyCast<ref_wrapper<T>>(value_).get();
        return SDEAnyCast<T>(value_);
    }

    template<typename T>
    T value() {
        if(is_ref_)
            return SDEAnyCast<ref_wrapper<T>>(value_).get();
        return SDEAnyCast<T>(value_);
    }
    //@}

    /** @brief Interface for changing the value held within
     *
     *  This function provides a convenient mechanism for setting the value or
     *  changing the already set value. Usage is like:
     *
     *  ```
     *  ModuleIO io;
     *  //Set-up io
     *  int new_value = 6;
     *  io.change(new_value); //<-Makes io hold a value of 6
     *  ```
     *
     *  @note The change will only occur if the input value passes all checks.
     *        Usually this means that it must minimally be of the right type.
     *
     * @tparam T The type of the input value.
     * @param new_value The value to make this instance hold. @p new_value will
     *        be subjected to all available checks.
     * @throw std::invalid_argument if @p new_value fails any checks. Strong
     *        throw guarantee.
     * @throw std::bad_alloc if there is insufficent memory to create the holder
     *        class. Strong throw guarantee.
     */
    template<typename T>
    void change(T&& new_value) {
        auto temp = wrap_value(std::forward<T>(new_value));
        if (!is_valid_(temp))
            throw std::invalid_argument("Not a valid option value");
        value_.swap(temp);
    }

    /** @brief Function for determining whether or not a value is acceptable.
     *
     * @tparam T The type of the value we are testing.
     * @param test_value The value to test.
     * @return True if the value this instance holds can be changed to
     *         @p test_value and false otherwise.
     * @throw std::bad_alloc if there is insufficient memory to create a holder
     *        class for the value. Strong throw guarantee.
     */
    template<typename T>
    bool is_valid(T &&test_value) const {
        return is_valid_(wrap_value(std::forward<T>(test_value)));
    }

    /** @brief Adds a callback that must return true before any new value is
     *         accepted.
     *
     * @tparam T The type of the value wrapped by this class.
     * @param check The callback to add
     * @param desc A description for this callback.
     * @throw std::bad_alloc if there is insufficient memory to copy the
     *        callback. Strong throw guarantee.
     */
    template<typename T>
    void add_check(value_checker<T> check, description_type desc = "") {
        if (desc == "")
            desc = "Check #" + std::to_string(checks_.size());
        auto lambda = [=](const any_type& value) {
            return check(SDEAnyCast<const T &>(value));
        };
        checks_[desc] = lambda;
    }

    /** @brief Function for declaring the type of the wrapped value.
     *
     * A lot of times we know the type of an input/output to the module's API,
     * but we don't know its value. This function allows us to declare what that
     * type should be. The result of such a declaration is a runtime check to
     * ensure that the user-provided type is of the correct type.
     *
     * @tparam T The type of this input/output value as it would show up in a
     *         function signature, *e.g.* `T=const int&` as opposed to `T=int`
     *         if you are only going to read the value and do not want to copy
     *         it.
     * @throw std::bad_alloc if there is insufficient memory to store the
     *        callback. Strong throw guarantee.
     */
    template<typename T>
    void set_type(){
        constexpr bool is_ref = std::is_reference_v<T>;
        constexpr bool is_const = std::is_const_v<no_ref<T>>;
        using clean_T = std::decay_t<T>;
        check_fxn lambda;
        if constexpr(is_ref && is_const)
            lambda = [=](const any_type& value) {
                return value.type() == typeid(ref_wrapper<const clean_T>);
            };
        else if(is_ref)
            lambda = [=](const any_type& value) {
                return value.type() == typeid(ref_wrapper<clean_T>);
            };
        else
            lambda = [=](const any_type& value) {
                return value.type() == typeid(T);
            };
        add_check_(lambda, "Type Checker"); //may throw, rest is nothrow
        is_ref_ = is_ref;
        is_const_ref_ = is_ref && is_const;
        type_ = [=]()->const std::type_info& { return typeid(T);};
    }

    ///A human-readable description of what this input/output does/is
    description_type desc = "";
private:
    ///Type of a map from descriptions to functions working with the check_fxns
    using check_map = Utilities::CaseInsensitiveMap<check_fxn>;

    ///Code factorization for wrapping a value in the holder class
    template<typename T>
    auto wrap_value(T&& test_value) const {
        using clean_T = std::decay_t<T>;
        any_type temp;
        if(is_const_ref_)
            temp = make_SDEAny<ref_wrapper<const clean_T>>(std::cref(test_value));
        else if(is_ref_)
            temp = make_SDEAny<ref_wrapper<clean_T>>(std::ref(test_value));
        else
            temp = make_SDEAny<clean_T>(std::forward<T>(test_value));
        return temp;
    }

    ///Code factorization and non-templated part of is_valid
    bool is_valid_(const any_type &test_value) const {
        for (auto[name, check] : checks_)
            if (!check(test_value)) return false;
        return true;
    }

    ///Non-template part of add_check
    void add_check_(check_fxn fxn, description_type desc) {
        checks_[desc] = fxn;
    }

    ///A map from descriptions of checks to the callbacks that perform them
    check_map checks_;

    ///The actual type-erased value we are holding
    any_type value_;

    ///Callback for returning the type (callback b/c of type_info's weird
    // semantics probably should switch to type_index)
    std::function<const std::type_info&()> type_ =
        [=]()->const std::type_info& {return typeid(nullptr);};

    ///True if we are wrapping a reference, false otherwise
    bool is_ref_ =  false;
    ///True if we are wrapping a read-only reference, false otherwise.
    bool is_const_ref_ = false;
};
}

/** @brief Class for wrapping the output of a module.
 *
 *  This class is literally a typedef of detail_::ModuleIO, hence see
 *  detail_::ModuleIO's documentation for more details.
 *
 */
using ModuleOutput = detail_::ModuleIO;

/** @brief Class wrapping the input to a module
 *
 *  This class extends detail_::ModuleIO by adding a couple of flags only
 *  relevant for the input. Hence the vast majority of this class's
 *  API/implementation comes from detail_::ModuleIO and as such users should
 *  consult detail_::ModuleIO's documentation for more details.
 */
class ModuleInput : public detail_::ModuleIO {
public:
    ///Whether an input is transparent (does not affect the output)
    bool is_transparent = false;

    ///Whether an input is optional (the module can run without a value)
    bool is_optional = false;
};


}; //End namespace
