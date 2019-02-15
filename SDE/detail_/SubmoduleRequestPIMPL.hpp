#pragma once
#include "SDE/Module.hpp"
#include <typeindex>

namespace SDE {
namespace detail_ {

/** @brief Class that implements the SubmoduleRequest class
 *
 * This class is a detail and should not be used directly by the end-user.
 * Interactions with this class should go through the available member functions
 * to the extent possible to ensure that proper error checking is occurring.
 */
struct SubmoduleRequestPIMPL {
    /// Type used to hold the module
    using module_ptr = std::shared_ptr<Module>;

    ///@{
    /** @name Ctor and Assignment operators
     *
     *  @param rhs The instance to copy/move from. For move operations @p rhs
     *         is in a valid, but otherwise undefined state.
     *  @throw std::bad_alloc is thrown by 2 and 3 if there is insufficient
     *         memory to copy the other instance. Strong throw guarantee.
     *  @throw none 1, 4, and 5 are no throw guarantee.
     */
    SubmoduleRequestPIMPL() = default;

    SubmoduleRequestPIMPL(const SubmoduleRequestPIMPL& rhs) :
      m_desc(rhs.m_desc),
      m_type(rhs.m_type),
      m_module(rhs.have_module() ? std::make_shared<Module>(*rhs.m_module) :
                                   nullptr) {}

    SubmoduleRequestPIMPL& operator=(const SubmoduleRequestPIMPL& rhs) {
        return *this = std::move(SubmoduleRequestPIMPL(rhs));
    }

    SubmoduleRequestPIMPL(SubmoduleRequestPIMPL&&) = default;

    SubmoduleRequestPIMPL& operator=(SubmoduleRequestPIMPL&&) = default;
    ///@}

    /// Standard dtor
    ~SubmoduleRequestPIMPL() = default;

    /// Makes a deep-copy on the heap
    auto clone() const {
        return std::make_unique<SubmoduleRequestPIMPL>(*this);
    }

    ///@{
    /** @name State inquiries
     *
     * Functions in this section make inquiries against the state of the
     * request. Respectively they determine if the request:
     *
     * - has been fulfilled
     * - specifies a property type
     * - is ready to be run
     * - is for the provided property type
     *
     * @param type The property type to compare the requested property type
     *        against
     * @return Inquiries return true if that inquiry is true and false
     *         otherwise.
     * @throw none 1, 2, and 3 are no throw guarantee.
     * @throw std::runtime_error 4 throws if the type has not been set.
     */
    bool have_module() const noexcept { return static_cast<bool>(m_module); }

    bool type_set() const noexcept {
        return m_type != std::type_index(typeid(std::nullptr_t));
    }

    bool ready() const noexcept {
        if(!type_set()) return false;
        if(!have_module()) return false;
        return m_module->ready();
    }

    bool check_type(const std::type_info& type) const {
        if(!type_set()) throw std::runtime_error("Type is not set");
        return m_type == std::type_index(type);
    }
    ///@}

    /// Returns the submodule after ensuring there is one
    const Module& value() const {
        if(!have_module()) throw std::runtime_error("Submodule is not ready");
        return *m_module;
    }

    /// Locks the submodule
    void lock() {
        if(!ready()) throw std::runtime_error("Can't lock a non-ready module");
        m_module->lock();
    }

    /// Sets the property type the submodule must satisfy
    void set_type(const std::type_info& type) {
        if(type_set()) throw std::runtime_error("Can't set type twice");
        m_type = std::type_index(type);
    }

    ///@{
    /** @brief Comparison operators
     *
     * Two SubmoduleRequestPIMPL instances are equivalent if they both:
     * - request the same property type,
     * - have the same description, and
     * - are not satisfied or are both satisfied with the same module
     *
     *
     * @param rhs The request to compare against
     * @return True if the comparision is true and false otherwise.
     * @throw ??? If the comparison between the two modules throws. Same throw
     *        guarantee.
     */
    bool operator==(const SubmoduleRequestPIMPL& rhs) const {
        const bool lmod = have_module();
        const bool rmod = rhs.have_module();
        if(lmod != rmod) return false;
        if(!lmod && !rmod)
            return std::tie(m_desc, m_type) == std::tie(rhs.m_desc, rhs.m_type);
        else
            return std::tie(m_desc, m_type, *m_module) ==
                   std::tie(rhs.m_desc, rhs.m_type, *rhs.m_module);
    }

    bool operator!=(const SubmoduleRequestPIMPL& rhs) const {
        return !((*this) == rhs);
    }
    ///@}

    ///@{
    /** @name SubmoduleRequest State
     *
     *  The members in this section are the state of the SubmoduleRequest.
     *  Respectively they are:
     *
     *  - A human-readable description of what the submodule is used for
     *  - The property type the submodule must satisfy
     *  - The actual submodule
     */
    type::description m_desc;
    std::type_index m_type = std::type_index(typeid(std::nullptr_t));
    module_ptr m_module;
    ///@}
};

} // namespace detail_
} // namespace SDE
