#pragma once
#include "Memoization.hpp"
#include "SDE/detail_/SDEAny.hpp"
#include "SDE/detail_/SDEAny.hpp"
#include <Utilities/Containers/CaseInsensitiveMap.hpp>
#include <Utilities/SmartEnum.hpp>
#include <algorithm>
#include <functional>
#include <stdexcept>
#include <string>

namespace SDE {
namespace detail_ {
class PyOption;
} // namespace detail_

// Forward declaration
template<typename T>
struct OptionHelper;

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
    ~Comparison() noexcept = default;

    /**
     * @brief Returns true if op(lhs,rhs) is true
     * @param lhs
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

/**
 * @brief Possible traits an option can have.
 *
 * transparent options are not hashed.
 * optional options do not have to be set for the module to run (e.g. print
 * levels)
 * non_default options have been changed from their default values.
 *
 */
DECLARE_SmartEnum(OptionTraits, transparent, optional, non_default);

/**
 * @brief Class holding the information about the options used in Parameters.
 *
 * Each Option may hold a value, description of that value, validity checks that
 * value must meet, and various traits.  The description, traits, and range
 * checks must be set when the option is created.  After an Option instance is
 * inserted into a Parameters instance only the value can be changed in a
 * meaningful way (via Parameters::change).  This reflects the fact that only
 * the module developer (and the Parameters instance) are in a position to
 * meaningfully change the traits, description, and validity checks for an
 * option.
 *
 * The Option class is responsible for hiding the types of the actual parameter
 * values.
 */

class Option {
    template<typename T>
    friend struct OptionHelper;
    /// Type of the checks as we store them in this Option
    using type_erased_functors =
      std::vector<std::function<bool(const detail_::SDEAny&)>>;

    /// Tells us if type @p T is some form of qualified Option type.
    template<typename T>
    using is_an_option_t = std::is_same<Option, std::decay_t<T>>;

    /// Uses SFINAE to disable main ctor from being used as copy/move ctor
    template<typename T>
    using disable_if_opt = typename std::enable_if_t<!is_an_option_t<T>::value>;

public:
    /// Type of the object holding the set of traits for this option
    using traits_set_type = std::set<OptionTraits>;

    /**
     * @brief Type of an std::vector filled with functors to be used for
     *        checking an option.
     *
     * This is the type of the checks that should be passed to the Options
     * instance.  Internally the Option class will take care of ensuring the
     * checks can be used with the appropriate types.
     *
     *
     * @tparam T The type of the element held within this instance.
     */
    template<typename T>
    using check_function_vector =
      typename OptionHelper<T>::check_function_vector;

    Option() = default;

    /**
     * @brief Creates a new Option
     *
     * The constructor will perform type erasure on the provided @p val
     * by storing it in a SDEAny. The value must also pass any @p checks
     * provided.
     *
     * @tparam T the type of the value for the Option
     * @param value value to be stored as SDEAny
     * @param desc brief description of the value
     * @param checks set of validity checks @p value must fulfill
     * @param ts set of traits which control hashing, defaults, etc.
     * @throw std::invalid_argument if @p value does not pass all @p checks.
     *        Strong throw guarantee.
     */
    template<typename T, typename X = disable_if_opt<T>>
    explicit Option(T&& value, const std::string& desc = "",
                    const check_function_vector<T>& checks = {},
                    const traits_set_type& ts              = {}) :
      description(desc),
      checks_(),
      traits(ts),
      value_() {
        OptionHelper<T>::make_option(*this, value, desc, checks, ts);
    }

    /**
     * @brief Frees up memory contained within this instance.
     *
     * @throw None. No throw guarantee.
     */
    ~Option() = default;

    /**
     * @brief Undoes the type-erasure of the Option and returns the value.
     * @tparam T The type of the value held within the Option instance.
     * @return The value.
     * @throw std::bad_cast if the option is not of type @p T.  Strong throw
     *        guarantee.
     */
    template<typename T>
    T get() const {
        return detail_::SDEAnyCast<T>(value_);
    }

    /**
     * @brief Runs a value through all the validity checks of an Option
     *
     * Will only return true if the value passes all the checks
     * contained within this instance.  Note that this function is no throw
     * guarantee because the first check ensures the type is correct and if it's
     * not will simply return false, preventing any remaining checks which could
     * possibly throw.
     *
     * @param[in] new_value the value to check.
     * @return a bool indicating whether the value passed all checks.
     * @throw none No throw guarantee.
     */
    template<typename T>
    bool is_valid(T new_value) const noexcept {
        for(auto& ci : checks_)
            if(!ci(detail_::SDEAny(new_value))) return false;
        return true;
    }

    /**
     * @brief Wraps the process of changing an option's value.
     *
     * @tparam T The type of the new option.
     * @param new_value The potential value
     * @throw std::invalid_argument If @p new_value does not satisfy all
     *        validity checks.
     *        std::bad_alloc if there is insufficient memory to wrap the
     *        new option.  Strong throw guarantee in both cases.
     */
    template<typename T>
    void change(T&& new_value) {
        OptionHelper<T>::change(*this, std::forward<T>(new_value));
    }

    /// A description of what this option represents
    std::string description;

    /// Traits which control hashing, note defaults, etc.
    traits_set_type traits;

    /// Hashes the value state of this option instance
    void hash(Hasher& h) const { h(value_, description, traits); }

    /**
     * @brief Option comparison operators
     *
     * Two options are defined as equal if they have the same value, the same
     * description, and the same traits.  We do not consider the checks to be
     * part of the Option's state.  Aside from the practical reason that one
     * can't compare `std::function` instances, this is motivated by the fact
     * that the checks can be seen as restricting the domain of possible
     * parameter values.
     *
     */
    ///@{
    bool operator==(const Option& rhs) const noexcept {
        return std::tie(value_, description, traits) ==
               std::tie(rhs.value_, rhs.description, rhs.traits);
    }

    bool operator!=(const Option& rhs) const noexcept {
        return !((*this) == rhs);
    }
    ///@}
protected:
    /// Allows Python helper class to get at data
    friend class detail_::PyOption;

    /// The actual wrapped value
    detail_::SDEAny value_;

    /// List of checks to run for any new value, stored in type-erased form
    type_erased_functors checks_;
};

/**
 * @brief Primary template used to implement Option constructor and
 * Option::change.
 *
 * @tparam T type of the value of the Option
 */
template<typename T>
struct OptionHelper {
    using check_function_vector = std::vector<std::function<bool(const T&)>>;
    static const void change(Option& opt, T&& new_value) {
        if(!opt.is_valid(new_value))
            throw std::invalid_argument("Not a valid option value");
        detail_::SDEAny(std::forward<T>(new_value)).swap(opt.value_);
    }

    static const void make_option(
      Option& opt, T& value, const std::string& desc,
      const Option::check_function_vector<T>& checks,
      const Option::traits_set_type& ts) {
        // Add a check to make sure the new value is the right type
        opt.checks_.push_back([=](const detail_::SDEAny& da_any) {
            return da_any.type() == typeid(const std::decay_t<T>&);
        });
        // Add whatever other checks the developer provided
        for(auto ci : checks)
            opt.checks_.push_back([=](const detail_::SDEAny& da_any) {
                return ci(detail_::SDEAnyCast<const std::decay_t<T>&>(da_any));
            });
        OptionHelper::change(opt, std::forward<T>(value));
    }
};

/**
 * @brief Specialization for Option constructor and option::get, called when T
 * is a string literal
 *
 */

template<std::size_t N>
struct OptionHelper<const char (&)[N]> {
    static const void change(Option& opt, std::string&& new_value) {
        if(!opt.is_valid(new_value))
            throw std::invalid_argument("Not a valid option value");
        detail_::SDEAny(std::forward<std::string>(new_value)).swap(opt.value_);
    }

    using check_function_vector =
      std::vector<std::function<bool(const std::string&)>>;

    static const void make_option(
      Option& opt, const char (&tmp_value)[N], const std::string& desc,
      const Option::check_function_vector<const char (&)[N]>& checks,
      const Option::traits_set_type& ts) {
        // Make std::string from c-string, minus the null terminator
        std::string value(tmp_value, N - 1);

        // Add a check to make sure the new value is the right type
        opt.checks_.push_back([=](const detail_::SDEAny& da_any) {
            return da_any.type() == typeid(const std::decay_t<std::string>&);
        });

        // Add whatever other checks the developer provided
        for(auto ci : checks)
            opt.checks_.push_back([=](const detail_::SDEAny& da_any) {
                return ci(detail_::SDEAnyCast<const std::decay_t<std::string>&>(
                  da_any));
            });

        OptionHelper::change(opt, std::forward<std::string>(value));
    }
};

namespace detail_ {
/**
 * @brief Primary template, called when T for Parameters::at is not Option<U> (U
 * being any type)
 *
 * The next two structs exist so that the function Parameters::at<> can return
 * either the value of an Option, or the Option itself depending on the template
 * parameter given to it.
 *
 * @tparam T type of the value of the Option
 */
template<typename T>
struct AtHelper {
    // static so we don't have to make an instance
    static const T get(const Option& opt) { return opt.get<T>(); }
};

/**
 * @brief Specialization for Option, called when T for Parameters::at is
 * Option
 *
 */
template<>
struct AtHelper<Option> {
    static const Option get(const Option& opt) { return opt; }
};

} // namespace detail_
} // namespace SDE
