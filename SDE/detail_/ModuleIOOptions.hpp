#include "SDE/ModuleIO.hpp"

namespace SDE {

/**
 * @brief Class which compares a given value against a threshold
 * @tparam T type of the Option value to check.
 * @tparam op function which accepts two Ts and returns a bool
 */
template<typename T, typename op>
struct Comparison {
    /// The threshold to compare against
    T rhs;

    /**
     * @brief Constructor which takes a threshold to compare against
     * @param thresh value to compare against
     */
    Comparison(const T thresh) : rhs(thresh){};

    ///Default dtor just frees up the memory used for the threshold
    ~Comparison() noexcept = default;

    /**
     * @brief Returns true if op(lhs,rhs) is true
     * @param lhs The value to compare against the threshold
     * @return bool from result of op(lhs,rhs)
     */
    bool operator()(const T& lhs) const { return op()(lhs, rhs); }
};

/// Comparison using std::greater
template<typename T>
using GreaterThan = Comparison<T, std::greater<T>>;

/// Comparison using std::less
template<typename T>
using LessThan = Comparison<T, std::less<T>>;

namespace detail_ {

/** @brief Code factorization for the ModuleInputOptions/ModuleOutputOptions
 *  classes.
 *
 *  This class implements the builder pattern for the ModuleIO class. Analogous
 *  builders are available for ModuleInput and ModuleOutput (respectively they
 *  are ModuleInputOptions and ModuleOutputOptions).
 *
 */
class ModuleIOOptions {
private:
    ///Helper type for removing the reference on a type
    template<typename T>
    using no_ref = std::remove_reference_t<T>;

    ///The type of a wrapped reference
    template<typename T>
    using ref_wrapper = std::reference_wrapper<no_ref<T>>;
public:
    ///The type of a human-readable description for the value
    using description_type = typename ModuleIO::description_type;

    ///The type of a callback for checking the validity of a new value
    template<typename T>
    using value_checker = typename ModuleIO::value_checker<T>;

    //@{
    /** @name Ctors and Assignment Operators
     *
     * These members create new ModuleIOOptions instances. Since this class is
     * only meant for constructing a ModuleInput/ModuleOutput instance we have
     * deleted all non-essential ctors/assignment operators. The remaining two
     * are the main ctor, which takes the instance whose state is being set and
     * the copy ctor (so we can return an instance from a function).
     *
     * 1) Default ctor
     * 2 and 3) Copy ctor/assignment operator
     * 4 and 5) Move ctor/assignment operator
     * 6)
     *
     * @param parent The ModuleIO instance that the current ModuleIOOptions
     *        instance is in charge of initializing.
     *
     * With the exception of 2 and 6 all ctor/assignment operators are deleted
     * to avoid unintentional usage.
     */
    ModuleIOOptions() = delete;

    ModuleIOOptions(const ModuleIOOptions &) = default;

    ModuleIOOptions &operator=(const ModuleIOOptions &) = delete;

    ModuleIOOptions(ModuleIOOptions &&) = delete;

    ModuleIOOptions &operator=(ModuleIOOptions &&) = delete;

    ModuleIOOptions(ModuleIO &parent) : parent_(&parent) {}
    //@}

    ///Basic dtor, nothing special aside from it being virtual
    virtual ~ModuleIOOptions() = default;

    ///Sets the type of the input/output to @p T
    template<typename T>
    ModuleIOOptions& type() { parent_->set_type<T>(); return *this; }

    ///Sets the description of the input/output to @p desc
    ModuleIOOptions& description(description_type desc) {
        parent_->desc = std::move(desc); return *this;
    }

    //@{
    /** @name Functions for adding error checks.
     *
     * The various overloads autogenerate the description based on the type of
     * the callback. This only works for the callbacks provided in this file.
     *
     * 1) GreaterThan overload
     * 2) LessThan overload
     * 3) Catch all overload.
     *
     * @tparam T The type of the value wrapped by the ModuleIO class.
     * @param new_check The check to add.
     * @return The current instance, containing the new check.
     * @throws std::b
     */
    template<typename T>
    ModuleIOOptions& check(GreaterThan<T> new_check) {
        auto desc =
            "Ensures value is greater than " + std::to_string(new_check.rhs);
        return check(new_check, desc);
    }

    template<typename T>
    ModuleIOOptions& check(LessThan<T> new_check) {
        auto desc =
            "Ensures value is less than " + std::to_string(new_check.rhs);
        return check(new_check, desc);
    }

    template<typename T>
    ModuleIOOptions& check(value_checker<T> new_check,
                           description_type desc = ""){
        parent_->add_check(new_check);
        return *this;
    }
    //@}

    ///Sets the default value to @p value, assuming @p value is valid.
    template<typename T>
    ModuleIOOptions& default_value(T&& value) {
        parent_->change(std::forward<T>(value));
        return *this;
    }

private:
    ///The instance whose state we are modifying
    ModuleIO* parent_;
};

/** @brief The class responsible for building a ModuleOutput instance.
 *
 *  This class is literally a wrapper over ModuleIOOptions. See ModuleIOOptions
 *  for more details on usage.
 */
class ModuleOutputOptions : private ModuleIOOptions {
public:
    ///Makes a new ModuleOutputOptions instance that fills in @p parent
    ModuleOutputOptions(ModuleOutput& parent) :
        ModuleIOOptions(parent), my_parent_(&parent) {}
    ///Copy ctor so instances can be returned
    ModuleOutputOptions(const ModuleOutputOptions& rhs) = default;

    //Pull base class's members into scope
    using ModuleIOOptions::type;
    using ModuleIOOptions::description;
    using ModuleIOOptions::check;
    using ModuleIOOptions::default_value;

private:
    ///Pointer to the ModuleOutput instance we're modifying (name differs from
    ///base class member to avoid shadowing).
    ModuleOutput* my_parent_;
};

/** @brief Class responsible for building a ModuleInput instance
 *
 * This class is little more than a wrapper over ModuleIOOptions. The slight bit
 * of additional functionality comes from the additional members in the
 * ModuleInput class.
 *
 * @note We use private inheritance to avoid this class being used
 * polymorphically.
 */
class ModuleInputOptions : private ModuleIOOptions {
public:
    ///Makes an instance that sets the state of @p parent.
    ModuleInputOptions(ModuleInput& parent) :
        ModuleIOOptions(parent), my_parent_(&parent) {}

    ///Copy ctor so we can return an instance from a function
    ModuleInputOptions(const ModuleInputOptions& rhs) = default;

    //Brings the base class's members into scope
    using ModuleIOOptions::type;
    using ModuleIOOptions::description;
    using ModuleIOOptions::check;
    using ModuleIOOptions::default_value;        

    ///Marks the input as transparent (does not influence hashing)
    ModuleInputOptions& transparent(){
        my_parent_->is_transparent = true;
        return *this;
    }

    ///Marks the input as opaque (influences hashing), this is the default
    ModuleInputOptions& opaque(){
        my_parent_->is_transparent = false;
        return *this;
    }

    ///Marks the input as optional (the module can run without a value)
    ModuleInputOptions& optional(){
        my_parent_->is_optional = true;
        return *this;
    }

    ///Marks the input as required for the module to run, this is the default
    ModuleInputOptions& required(){
        my_parent_->is_optional = false;
        return *this;
    }
private:
    ///The instance we are setting the state of
    ModuleInput* my_parent_;
};
} // namespace detail_
} // namespace SDE
