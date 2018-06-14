#pragma once
#include "SDE/Memoization.hpp"
#include "SDE/SDEAny.hpp"
#include <Utilities/Containers/CaseInsensitiveMap.hpp>
#include <Utilities/SmartEnum.hpp>
#include <algorithm>
#include <functional>
#include <stdexcept>
#include <string>

namespace SDE {

/**
 * @brief Class which checks whether a given value is above a threshold
 * @tparam T type of the Option value to check.
 */
template<typename T>
struct GreaterThan {
    T low;

    GreaterThan(const T thresh) : low(thresh){};
    ~GreaterThan() noexcept = default;

    bool operator()(const detail_::SDEAny& value) { return detail_::SDEAnyCast<T>(value) > low; }
};

/**
 * @brief Class which checks whether a given value is below a threshold.
 * @tparam T type of the Option value to check.
 */
template<typename T>
struct LessThan {
    T high;

    LessThan(const T thresh) : high(thresh){};
    ~LessThan() noexcept = default;

    bool operator()(const detail_::SDEAny& value) { return detail_::SDEAnyCast<T>(value) < high; }
};

/**
 * @brief Class which checks whether a given value is between two thresholds.
 * @tparam T type of the Option value to check.
 */
template<typename T>
struct Between {
    T low;
    T high;

    Between(const T thresh1, const T thresh2) : low(thresh1), high(thresh2){};
    ~Between() noexcept = default;

    bool operator()(const detail_::SDEAny& value) {
        return (detail_::SDEAnyCast<T>(value) > low &&
               detail_::SDEAnyCast<T>(value) < high); }
};

/**
 * @brief Possible traits an option can have.
 *
 * transparent options are not hashed.
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
class Option {
    using check_function = std::function<bool(const detail_::SDEAny&)>;

public:
    /// Actual value used by modules
    detail_::SDEAny value;
    /// A brief description of the value.
    std::string description;
    /// Set of validity checks value must fulfill
    std::vector<check_function> range_checks;
    /// Traits which control hashing, note defaults, etc.
    std::vector<OptionTraits> traits;

    template<typename T>
    explicit Option(T&& val, std::string description = "",
           std::vector<check_function> range_checks = {},
           std::vector<OptionTraits> traits         = {}) :
      value(val),
      description(description),
      range_checks(range_checks),
      traits(traits) {
        if(!is_valid(value))
            throw std::invalid_argument("Not a valid option value");
    }
    Option() = default;
    ~Option() noexcept = default;

    /**
     * @brief Provides a hash of the Option's state
     *
     */
    void hash(Hasher& h) const { h(value); h(traits); }

    /**
     * @brief Runs a value through all the validity checks of an Option
     *
     * Will only return true if the value passes all the checks
     * contained in range_checks.
     *
     * @param val the value to check.
     * @return a bool indicating whether the value passed all checks.
     */
    bool is_valid(const detail_::SDEAny& val) {
        bool rv = true;
        for(const auto& check : range_checks) {
            if(!check(val)) rv = false;
        }
        return rv;
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
    static const T& get(const Option& opt) {
        return SDEAnyCast<T>(opt.value);
    }
};

/**
 * @brief Specialization for Option, called when T for Parameters::at is
 * Option
 *
 */
template <>
struct AtHelper<Option> {
    static const Option& get(const Option& opt) {
        return opt;
    }
};
} // namespace detail_

/**
 * @brief Class for holding the parameters to a module
 */
class Parameters {
public:
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
     * @param new_value the new value of the Option.
     */
    template<typename T>
    void change(std::string key, const T new_val) {
        detail_::SDEAny new_value(new_val);
        auto opt = options.at(key);
        if(!opt.is_valid(new_value))
            throw std::invalid_argument("Not a valid option value");
        opt.value = new_value;

        if(tracking_changes &&
           (std::find(opt.traits.begin(), opt.traits.end(),
                      OptionTraits::non_default) == opt.traits.end()))
            opt.traits.push_back(OptionTraits::non_default);
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
    void insert(std::string key, Option opt) {
        if(std::find(opt.traits.begin(), opt.traits.end(),
                     OptionTraits::transparent) == opt.traits.end()) {
            keys_to_hash.insert(key);
        }
        options[key] = opt;
    }

    /**
     * @brief Returns whether the options map contains a particular key.
     *
     * @param key the string we are checking for in the options map.
     * @return a bool which is true if the options map contains the key.
     */
    std::size_t count(std::string key) const noexcept { return options.count(key); }

    /**
     * @brief Gives the value of the option at a particular key.
     *
     * Casts the SDEAny option to the appropriate type and returns the value.
     *
     * @tparam T the type of the value.
     * @param key the string mapped to the option we want the value of.
     * @return the value of.
     */
    template<typename T>
    const T& at(const std::string& key) const {
        return detail_::AtHelper<T>::get(options.at(key));
    }

    /**
     * @brief Hashes the non-transparent Parameter options.
     */
    void hash(Hasher& h) const {
        for(const auto& key : keys_to_hash) { h(key); h(options.at(key)); }
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
