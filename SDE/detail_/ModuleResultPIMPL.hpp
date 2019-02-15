#pragma once
#include "SDE/Types.hpp"
#include "SDE/detail_/SDEAny.hpp"
#include <memory>
#include <typeindex>

namespace SDE {
namespace detail_ {

/** @brief The class responsible for implementing the ModuleResult class
 *
 *  Like the ModuleInput class, this class is not meant to be used directly. If
 *  you must use this class it is recommended that you use the member functions
 *  as much as possible to benefit from error checking.
 *
 */
struct ModuleResultPIMPL {
    /// The type of a shared_ptr to a read-only SDEAny
    using shared_any = std::shared_ptr<const type::any>;

    /// Makes a copy of this class on the heap
    auto clone() const { return std::make_unique<ModuleResultPIMPL>(*this); }

    /// Returns true if the type is set and false otherwise
    bool is_type_set() const noexcept {
        return m_type != std::type_index(typeid(std::nullptr_t));
    }

    /// Determines if the wrapped value is valid
    bool is_valid(const type::any& value) const {
        if(!is_type_set()) throw std::runtime_error("Type must be set first");
        if(!value.has_value()) return false;
        return std::type_index(value.type()) == m_type;
    }

    ///@{
    /** @name State setters.
     *
     * @param new_xxx The value the piece of state should be set to.
     * @throw std::runtime_error is thrown by 1 if the type is already set and
     *        by 2 if the type has not been set yet. Strong throw guarantee.
     * @throw std::invalid_argument is thrown by 2 if the type of the value is
     *        not the same as the developer specified. Strong throw guarantee.
     */
    void set_type(const std::type_info& new_type) {
        if(is_type_set()) throw std::runtime_error("Can't set type twice");
        m_type = std::type_index(new_type);
    }

    void change(shared_any new_value) {
        if(!is_type_set()) throw std::runtime_error("Type must be set first.");
        if(!is_valid(*new_value))
            throw std::invalid_argument("Value has failed one or more checks.");
        m_value = new_value;
    }

    ///@{
    /** @name Equality comparisons
     *
     * Two ModuleResultPIMPL instances are equivalent if they hold the same
     * value, have the same description of that value, and require the same
     * type.
     *
     * @param rhs The instance to compare to.
     * @return Whether the specified comparison is true or not.
     * @throw ??? Throws if the value's comparison throws. Same guarantee.
     */
    bool operator==(const ModuleResultPIMPL& rhs) const {
        return std::tie(m_value, m_desc, m_type) ==
               std::tie(rhs.m_value, rhs.m_desc, rhs.m_type);
    }

    bool operator!=(const ModuleResultPIMPL& rhs) const {
        return !((*this) == rhs);
    }
    ///@}

    ///@{
    /** @names State
     *
     * The members in this section are the state of the ModuleResult instance.
     * Respectively they store:
     *
     * - The type-erased value of this return
     * - A description of the value
     * - The type the value must adhere to
     */
    shared_any m_value;
    type::description m_desc = "";
    std::type_index m_type   = std::type_index(typeid(std::nullptr_t));
    ///@}
};

} // namespace detail_
} // namespace SDE
