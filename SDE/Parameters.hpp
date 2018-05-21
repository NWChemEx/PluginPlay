#pragma once
#include "SDE/SDEAny.hpp"
#include "SDE/Memoization.hpp"
#include <Utilities/Containers/CaseInsensitiveMap.hpp>
#include <Utilities/SmartEnum.hpp>
#include <string>
#include <stdexcept>

namespace SDE {

template<typename T>
struct OptionChecker {
    virtual ~OptionChecker(){}
    virtual bool operator()(T& value) = 0;

    void hash(Hasher& h) const {hash_(h);}
    virtual void hash_(Hasher& h) const = 0;
};

template<typename T>
struct GreaterThan : OptionChecker<T> {
    T low;

    GreaterThan(const T thresh) : low(thresh) {};
    ~GreaterThan() = default;

    bool operator()(T& value) override {
        return value > low;
    }

    virtual void hash_(Hasher& h) const override { h(low); }
};

template<typename T>
struct LessThan : OptionChecker<T> {
    T high;

    LessThan(const T thresh) : high(thresh) {};
    ~LessThan() = default;

    bool operator()(T& value) override {
        return value < high;
    }

    virtual void hash_(Hasher& h) const override { h(high); }
};

template<typename T>
struct Between : OptionChecker<T> {
    T low;
    T high;

    Between(const T thresh1, const T thresh2) : low(thresh1), high(thresh2) {};
    ~Between() = default;

    bool operator()(T& value) override {
        return (value > low && value < high);
    }

    virtual void hash_(Hasher& h) const override { h(low,high); }
};

/**
 * @brief A list of traits an option can have
 *
 */
// TODO add hash free function for SmartEnum
//DECLARE_SmartEnum(OptionTraits, transparent, optional);
enum class OptionTraits{transparent, optional};

template<typename T>
class Option {
public:

    T value;
    std::string description;
    std::vector<std::unique_ptr<OptionChecker<T>>> range_checks;
    std::vector<OptionTraits> traits;

    Option(T value,
           std::string description = "",
           std::vector<std::unique_ptr<OptionChecker<T>>> range_checks = {},
           std::vector<OptionTraits> traits = {}) : value(value),
                                                    description(description),
                                                    range_checks(std::move(range_checks)),
                                                    traits(traits) {
        check_option();
    }
    explicit Option();
    Option(const Option&) = delete;
    Option& operator=(const Option&) = delete;
    ~Option() = default;

    void hash(Hasher& h) const { h(value,description,range_checks,traits); }

    void check_option() {
        for (auto& check : range_checks) {
            if (!(*check)(value)) throw std::invalid_argument("");
        }
    }

};

/**
 * @brief Class for holding the parameters to a module
 */
class Parameters {

    Utilities::CaseInsensitiveMap<SDE::detail_::SDEAny> options;

public:

    /**
     * @brief
     *
     * @tparam T
     * @param key
     * @param new_value
     */
    template<typename T>
    void change(std::string key, const T new_value) {
        auto& opt = SDE::detail_::SDEAnyCast<Option<T>>(options.at(key));
        opt.value = new_value;
        opt.check_option();
        insert(key,opt);
    }

    template<typename T>
    void insert(std::string key, Option<T> opt) {
        options[key] = SDE::detail_::make_SDEAny<Option<T>>(opt);
    }

    bool count(std::string key) {return options.count(key);}

    template<typename T>
    const T& at(std::string key) {
        return (SDE::detail_::SDEAnyCast<Option<T>>(options.at(key))).value;
    }

    //void hash(Hasher& h) const {h(options);}
};



}