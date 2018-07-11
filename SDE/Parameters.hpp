#pragma once
#include "SDE/Memoization.hpp"
#include "SDE/SDEAny.hpp"
#include "SDEAny.hpp"
#include <Utilities/Containers/CaseInsensitiveMap.hpp>
#include <Utilities/SmartEnum.hpp>
#include <algorithm>
#include <functional>
#include <stdexcept>
#include <string>

namespace SDE {
namespace detail_ {
class PyOption;
class PyParameters;
} // namespace detail_

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
    using check_function_vector = std::vector<std::function<bool(const T&)>>;

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
        // Add a check to make sure the new value is the right type
        checks_.push_back([=](const detail_::SDEAny& da_any) {
            return da_any.type() == typeid(std::decay_t<T>);
        });
        // Add whatever other checks the developer provided
        for(auto ci : checks)
            checks_.push_back([=](const detail_::SDEAny& da_any) {
                return ci(detail_::SDEAnyCast<const std::decay_t<T>&>(da_any));
            });
        change(std::forward<T>(value));
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
        if(!is_valid(new_value))
            throw std::invalid_argument("Not a valid option value");
        detail_::SDEAny(std::forward<T>(new_value)).swap(value_);
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

/**
 * @brief Class for holding the parameters to a module
 */
class Parameters {
public:
    /// A type used for returns resulting in a natural number (including zero)
    using size_type = std::size_t;

    /// The type of the returned set of traits
    using traits_set_type = typename Option::traits_set_type;

    /**
     * @brief Creates an empty Parameters
     *
     * @throws None. No throw guarantee.
     */
    Parameters() = default;

    /**
     * @brief Convenience ctor to avoid having to call insert for every option.
     *
     * Note that the Args must be of the form std::string, Option, std::string,
     * Option, .... That is Args must contain an even number of types that
     * alternate between a string (the key) and the corresponding Option.
     * Failure to follow this convention will lead to a compiler error.
     *
     * @tparam Args The types of the remaining key, option pairs.
     * @param key The key to associate with the option.
     * @param opt1  The option associated with the key.
     * @param args The remaining key, option pairs.
     * @throw std::bad_alloc if there is insufficient memory to store the
     *        options.  Strong throw guarantee.
     */
    template<typename... Args>
    Parameters(const std::string& key, Option opt1, Args&&... args) :
      Parameters(std::forward<Args>(args)...) {
        insert(key, opt1);
    }

    /**
     * @brief Frees up memory associated with this Parameters instance.
     * @throw None. No throw guarnatee.
     */
    ~Parameters() = default;

    /**
     * @brief Change the value of an Option in the Parameters.
     *
     * This function will change the value of an existing Option,
     * after first checking the new value against the validity
     * checks.
     *
     * @tparam T the type of the value of the Option.
     * @param key the string which maps to the Option to be changed.
     * @param new_val the new value of the Option.
     * @throw std::invalid_argument if @p new_value does not pass all
     * range_checks for the Option at @p key.
     * @throw std::out_of_range if @p key is not a valid key.
     *        std::invalid_argument if @p new_val does not satisfy all
     *        validity checks.  Strong throw guarantee.
     */
    template<typename T>
    void change(const std::string& key, const T& new_val) {
        auto& opt = options_.at(key); // Note we grab the opt by reference
        opt.change(new_val);
        if(tracking_changes_) opt.traits.insert(OptionTraits::non_default);
    }

    /**
     * @brief Function to insert a new Option into the Parameters class.
     *
     * Given a string key/Option pair, will add this pair to the options map.
     *
     * @tparam T type of the value of the Option.
     * @param key the string being mapped to the Option.
     * @param opt the Option to add to the Parameters.
     * @throw std::bad_alloc if there is insufficient memory to add the option.
     *        Strong throw guarantee.
     */
    void insert(const std::string& key, Option opt) {
        if(opt.traits.count(OptionTraits::transparent))
            transparent_.insert(key);
        options_[key] = opt;
    }

    /**
     * @brief Returns the number of options with the given key.
     *
     * @param key the string we are checking for in the options map.
     * @return the number of times @p key is contained within this instance.
     *         Will always be either 0 or 1.
     * @throw None No throw guarantee.
     */
    size_type count(const std::string& key) const noexcept {
        return options_.count(key);
    }

    /**
     * @brief Returns the total number of parameters this instance is
     * holding.
     * @throw None No throw guarantee.
     */
    size_type size() const noexcept { return options_.size(); }

    /**
     * @brief Gives the value of the Option, or the Option itself, at a
     * particular key.
     *
     * Casts the SDEAny option to the appropriate type and returns the value.
     * Will return the entire Option if T is type Option.
     *
     * @tparam T the type of the value to return. If T is type Option, return
     * the entire Option.
     * @param key the string mapped to the option or value we want.
     * @return the value of the Option at @p key, or the Option itself.
     * @throw std::out_of_range if @p key is not a valid key. Strong throw
     *        guarantee.
     */
    template<typename T>
    T at(const std::string& key) const {
        static_assert(std::is_same<std::decay_t<T>, T>::value,
                      "Template parameter should be unqualified");
        return detail_::AtHelper<T>::get(options_.at(key));
    }

    /**
     * @brief Convenience function to get the description of an Option
     *        held in a Parameters object
     *
     *
     * @param key the string mapped to the option we want.
     * @return the description of the option at @p key
     * @throw std::range_error if @p key is not a valid key.  Strong throw
     *        guarantee.
     */
    const std::string& get_description(const std::string& key) const {
        return options_.at(key).description;
    }

    /**
     * @brief Convenience function to get the traits of an Option
     *        held in a Parameters object
     * @param key the string mapped to the option we want.
     * @return the traits of the option at @p key
     * @throw std::range_error if @p key is not a valid key.  Strong throw
     *        guarantee.
     */
    const traits_set_type& get_traits(const std::string& key) const {
        return options_.at(key).traits;
    }

    /**
     * @brief Hashes the non-transparent Parameter options.
     */
    void hash(Hasher& h) const {
        for(const auto& x : options_)
            if(!transparent_.count(x.first)) h(x);
    }

    /**
     * @brief Turns on flag to start recording which Options have changed.
     *
     * @throw None No throw guarantee.
     */
    void track_changes() noexcept { tracking_changes_ = true; }

    /**
     * @ingroup Comparison Operators
     *
     * Operators for comparing Parameters instances.
     *
     * For a Parameters instance, equality is defined as having the same
     * list of options, under the same keys, with the same values.  The list
     * of transparent parameters is an internal convenience and is accounted
     * for in the above comparisons.  Whether we're tracking changes or not is
     * a bit of a grey area.  For present we have opted not to consider this,
     * but note that if any options have changed since turning on tracking they
     * will compare different.
     */
    ///@{
    bool operator==(const Parameters& rhs) const noexcept {
        return options_ == rhs.options_;
    }

    bool operator!=(const Parameters& rhs) const noexcept {
        return !((*this) == rhs);
    }
    ///@}

protected:
    /// Allows Python helper class to get at data
    friend class detail_::PyParameters;
    /// Flag indicating that future changes to Parameters will be tracked.
    bool tracking_changes_ = false;

private:
    /// Set of  keys not to hash
    std::unordered_set<std::string> transparent_;

    // The set of all the options
    Utilities::CaseInsensitiveMap<Option> options_;
};

} // end namespace SDE
