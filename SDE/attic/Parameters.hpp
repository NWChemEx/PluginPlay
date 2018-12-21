#pragma once
#include "Memoization.hpp"
#include "Option.hpp"
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
class PyParameters;
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
