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
    bool operator()(const detail_::SDEAny& lhs) const {
        return op()(detail_::SDEAnyCast<T>(lhs), rhs);
    }
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
 * value must meet, and various traits.
 *
 */
struct Option {
    using check_function = std::function<bool(const detail_::SDEAny&)>;

    /// Actual value used by modules
    detail_::SDEAny value;
    /// A brief description of the value.
    std::string description;
    /// Set of validity checks value must fulfill
    std::vector<check_function> range_checks;
    /// Traits which control hashing, note defaults, etc.
    std::set<OptionTraits> traits;

    /**
     * @brief Creates a new Option
     *
     * The constructor will perform type erasure on the provided @p val
     * by storing it in a SDEAny. The value must also pass any @p range_checks
     * provided.
     *
     * @tparam T the type of the value for the Option
     * @param val value to be stored as SDEAny
     * @param description brief description of the value
     * @param range_checks set of validity checks val must fulfill
     * @param traits set of traits which control hashing, defaults, etc.
     * @throw std::invalid_argument if @p val does not pass all @p range_checks.
     */
    template<typename T>
    explicit Option(T&& val, std::string description = "",
                    std::vector<check_function> range_checks = {},
                    std::set<OptionTraits> traits            = {}) :
      value(val),
      description(description),
      range_checks(range_checks),
      traits(traits) {
        if(!is_valid()) throw std::invalid_argument("Not a valid option value");
    }

    /**
     * @brief Creates an empty Option.
     *
     * @throws None. No throw guarantee.
     */
    Option()           = default;
    ~Option() noexcept = default;

    /**
     * @brief Provides a hash of the Option's state
     *
     */
    void hash(Hasher& h) const { h(value)/*, traits)*/;}

    /**
     * @brief Runs a value through all the validity checks of an Option
     *
     * Will only return true if the value passes all the checks
     * contained in range_checks.
     *
     * @param val the value to check.
     * @return a bool indicating whether the value passed all checks.
     */
    bool is_valid() {
        for(const auto& check : range_checks) {
            if(!check(this->value)) return false;
        }
        return true;
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
    static const T& get(const Option& opt) { return SDEAnyCast<T>(opt.value); }
};

/**
 * @brief Specialization for Option, called when T for Parameters::at is
 * Option
 *
 */
template<>
struct AtHelper<Option> {
    static const Option& get(const Option& opt) { return opt; }
};

/**
 * @brief Specialization for pyobject, called when T for Parameters::at is a
 * pyobject
 */
/*template<>
struct AtHelper<pyobject> {
    static const pyobject& get (const Option& opt) { return
opt.value.pythonize(); }
};*/

/*
template<typename T>
struct ChangeHelper {
    static void change_opt(Option& opt, const T& new_value) {
        opt.value = detail_::SDEAny(new_value);
        if(!opt.is_valid())
            throw std::invalid_argument("Not a valid option value");
    }
};

template<>
struct ChangeHelper<pyobject> {
    static void change_opt(Option& opt, pyobject& new_value) {
        opt.value.insert_python(new_value);
        if(!opt.is_valid())
            throw std::invalid_argument("Not a valid option value");
    }
};*/
} // namespace detail_

/**
 * @brief Class for holding the parameters to a module
 */
class Parameters {
public:
    /**
     * @brief Creates an empty Parameters
     *
     * @throws None. No throw guarantee.
     */
    Parameters()           = default;
    ~Parameters() noexcept = default;

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
     * @throw range_error if @p key is not a valid key
     */
    template<typename T>
    void change(const std::string& key, const T& new_val) {
        auto opt  = options.at(key);
        opt.value = detail_::SDEAny(new_val);
        if(!opt.is_valid())
            throw std::invalid_argument("Not a valid option value");

        if(tracking_changes) opt.traits.insert(OptionTraits::non_default);
        insert(key, opt);
    }

    /**
     * @brief Allows changing the Parameters from Python
     *
     *        Takes a pybind11::object and puts it into the SDEAny
     *        value stored by an Option.
     *
     * @param key the string which maps to the Option to be changed.
     * @param new_val the new value of the Option.
     * @throw std::invalid_argument if @p new_value does not pass all
     * range_checks for the Option at @p key.
     * @throw range_error if @p key is not a valid key
     */
    void change_python(const std::string& key, pyobject new_val) {
        auto opt = options.at(key);
        opt.value.insert_python(new_val);
        if(!opt.is_valid())
            throw std::invalid_argument("Not a valid option value");

        if(tracking_changes) opt.traits.insert(OptionTraits::non_default);
        insert(key, opt);
    }

    /**
     * @brief Function to insert a new Option into the Parameters class.
     *
     * Given a string key/Option pair, will add this pair to the options map.
     *
     * @tparam T type of the value of the Option.
     * @param key the string being mapped to the Option.
     * @param opt the Option to add to the Parameters.
     */
    void insert(const std::string& key, Option opt) {
        if(!opt.traits.count(OptionTraits::transparent))
            keys_to_hash.insert(key);
        options[key] = opt;
    }

    /**
     * @brief Returns whether the options map contains a particular key.
     *
     * @param key the string we are checking for in the options map.
     * @return a bool which is true if the options map contains the key.
     */
    std::size_t count(const std::string& key) const noexcept {
        return options.count(key);
    }

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
     * @throw range_error if @p key is not a valid key
     */
    template<typename T>
    const T& at(const std::string& key) const {
        return detail_::AtHelper<T>::get(options.at(key));
    }

    /**
     * @brief Returns the value of an Option as a pybind::11 object
     * @param key the string mapped to the value we want.
     * @return the value of the Option at @p key as a pybind11::object
     * @throw range_error if @p key is not a valid key
     */
    pyobject at_python(const std::string& key) const {
        return options.at(key).value.pythonize();
    }

    /**
     * @brief Convenience function to get the description of an Option
     *        held in a Parameters object
     *
     *
     * @param key the string mapped to the option we want.
     * @return the description of the option at @p key
     * @throw range_error if @p key is not a valid key
     */
    const std::string& get_description(const std::string& key) const {
        return options.at(key).description;
    }

    /**
     * @brief Convenience function to get the traits of an Option
     *        held in a Parameters object
     * @param key the string mapped to the option we want.
     * @return the traits of the option at @p key
     * @throw range_error if @p key is not a valid key
     */
    const std::set<OptionTraits>& get_traits(const std::string& key) const {
        return options.at(key).traits;
    }

    /**
     * @brief Hashes the non-transparent Parameter options.
     */
    void hash(Hasher& h) const {
        for(const auto& key : keys_to_hash) h(key, options.at(key));
    }

    /**
     * @brief Turns on flag to start recording which Options have changed.
     */
    void track_changes() { tracking_changes = true; }

private:
    // Map of Options which have been type-erased
    Utilities::CaseInsensitiveMap<Option> options;
    // List of keys to be hashed from the options map, does not include
    // transparent Options
    std::unordered_set<std::string> keys_to_hash;
    // Flag indicating that future changes to Parameters will be tracked.
    bool tracking_changes;
};

} // end namespace SDE
