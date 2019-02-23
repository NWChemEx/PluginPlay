#pragma once
#include "SDE/Types.hpp"
#include "SDE/detail_/SDEAny.hpp"
#include <Utilities/Containers/CaseInsensitiveMap.hpp>
#include <typeindex>

namespace SDE {
namespace detail_ {

/** @brief Implements the ModuleInput class
 *
 *  This class contains the details of how the ModuleInput class actually works
 *  whereas the ModuleInput class largely defines the API that users go through
 *  to use a ModuleInput. The fact that it lives in the detail_ namespace means
 *  that you shouldn't be directly using this class. If you are using this class
 *  you should go through the member functions as much as possible to ensure
 *  error checking.
 *
 *  @note If an additional implementation is needed the API should be extended
 *        to getters/setters for the member values and this implementation moved
 *        into a derived class.
 */
struct ModuleInputPIMPL {
    /// The type of the type-erased functors used for checking a value
    using any_check = std::function<bool(const type::any&)>;

    /// Creates a deep copy of the current class on the heap
    auto clone() const { return std::make_unique<ModuleInputPIMPL>(*this); }

    /// Checks if the input is optional or if it has a value
    bool is_ready() const noexcept { return m_optional || m_value.has_value(); }

    /// Checks if the provided value is valid
    bool is_valid(const type::any& new_value) const {
        if(!is_type_set()) throw std::runtime_error("Must set type first");
        for(const auto & [k, v] : m_checks)
            if(!v(new_value)) return false;
        return true;
    }

    /// Checks if the type has been set
    bool is_type_set() const noexcept {
        return m_type != std::type_index(typeid(std::nullptr_t));
    }

    /// Computes a hash value for the state of this class
    void hash(type::hasher& h) {
        if(!m_transparent) h(m_value);
    }

    ///@{
    /** @name State Modifiers
     *
     * The functions in this section change the state of the PIMPL and perform
     * some error checking while doing it. Respectively:
     *
     * - Sets the type of this input field
     * - Adds a check that the value must satisfy
     * - Changes the value
     *
     * @param new_xxx The value to change the state to
     * @throw std::runtime_error 1 throws if the type is already set (the type
     *        is set in the module's ctor, where we assume that the developer
     *        knows what type they need). 3 throws if the type has not been set
     *        yet. Strong throw guarantee.
     * @throw std::bad_alloc 2 throws if there is insufficient memory to create
     *        a description (assuming one has not been provided), or if there
     *        is not enough memory to store the check. Strong throw guarantee.
     */
    void set_type(const std::type_info& new_type) {
        if(is_type_set()) { throw std::runtime_error("Can't set type twice"); }
        m_type = std::type_index(new_type);
    }

    void add_check(any_check new_check, type::description new_desc = "") {
        if(new_desc == "") {
            new_desc = "Check #";
            new_desc += std::to_string(m_checks.size());
        }
        m_checks.emplace(std::move(new_desc), std::move(new_check));
    }

    void change(type::any new_value) {
        if(!is_type_set()) throw std::runtime_error("Must set type first");
        m_value.swap(new_value);
    }

    ///@}

    ///@{
    /** @name Comparision operators
     *
     *  Two ModuleInputPIMPL instances are equivalent if their states are
     *  equivalent. This includes the value they hold, their descriptions, the
     *  type they are supposed to hold, whether they are optional, and whether
     *  they are transparent. There is no feasible way to compare the checks so
     *  they omitted from the consideration.
     *
     *  Respectively the operators:
     *
     *  - Compare for equivalence
     *  - Compare for difference
     *
     *  @return true if the requested comparison is true and false otherwise.
     *  @throw none No throw guarantee.
     */
    bool operator==(const ModuleInputPIMPL& rhs) const noexcept {
        return std::tie(m_value, m_desc, m_optional, m_transparent, m_type) ==
               std::tie(rhs.m_value, rhs.m_desc, rhs.m_optional,
                        rhs.m_transparent, rhs.m_type);
    }

    bool operator!=(const ModuleInputPIMPL& rhs) const noexcept {
        return !((*this) == rhs);
    }
    ///@}

    ///@{
    /** @name State
     *
     *  These members represent the entire state of an input. Respectively they
     *  are:
     *
     *  - The type-erased value
     *  - A description of the value
     *  - Whether the value is optional (the module can run without it)
     *  - Whether the value influences the outcome
     *  - The checks a new value must satisfy
     *  - The type the value will be used as.
     */
    type::any m_value;
    type::description m_desc = "";
    bool m_optional          = false;
    bool m_transparent       = false;
    Utilities::CaseInsensitiveMap<any_check> m_checks;
    std::type_index m_type = std::type_index(typeid(std::nullptr_t));
    ///@}
};

} // namespace detail_
} // namespace SDE
