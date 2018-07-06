#pragma once
#include "SDE/Parameters.hpp"

namespace SDE {

struct PyParameters : Parameters {
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
     * @brief Returns the value of an Option as a pybind::11 object
     * @param key the string mapped to the value we want.
     * @return the value of the Option at @p key as a pybind11::object
     * @throw range_error if @p key is not a valid key
     */
    pyobject at_python(const std::string& key) const {
        return options.at(key).value.pythonize();
    }
};

} // end namespace SDE
