#pragma once
#include "sde/detail_/archive_wrapper.hpp"
#include "sde/detail_/sde_any.hpp"
#include "sde/serialization.hpp"
#include "sde/types.hpp"
#include <memory>
#include <typeindex>
#include <utilities/printing/demangler.hpp>

namespace sde::detail_ {

/** @brief The class responsible for implementing the ModuleResult class
 *
 *  This class is very similar to ModuleInputPIMPL except that it is geared at
 *  the values that are returned form a module. Handeling the inputs is a lot
 *  more tricky than the results because the result are immutable and always
 *  passed around as shared pointers. Thus this class is also much simpler in
 *  implementation ModuleInputPIMPL.
 *
 */
class ModuleResultPIMPL {
public:
    /// The type of a shared_ptr to a read-only SDEAny
    using shared_any = std::shared_ptr<const type::any>;

    /** @brief Makes a deep copy of the PIMPL on the heap.
     *
     *  This function is primarily for use by the ModuleResult class to easily
     *  copy the PIMPL.
     *
     *  @return A unique_ptr to the new PIMPL.
     *
     *  @throw std::bad_alloc if there is insufficient memory to copy this
     *                        instance. Strong throw guarantee.
     */
    auto clone() const { return std::make_unique<ModuleResultPIMPL>(*this); }

    /** @brief Has the type of this result field been set?
     *
     *  Each result field holds an object of a given type. This function can be
     *  used to determine if the type of the present field has been set.
     *
     *  @return True if this field's type has been set and false otherwise.
     *
     *  @throw none No throw gurantee.
     */
    bool has_type() const noexcept { return m_type_.has_value(); }

    /** @brief Has a value been bound to this field yet?
     *
     *  At any given time a result field either has a value bound to it or it
     *  does not. This function can be used to determine which state the field
     *  is in.
     *
     *  @return true if the this field has a value and false otherwise.
     *
     *  @throw none No throw guarantee.
     */
    bool has_value() const noexcept { return static_cast<bool>(m_value_); }

    /** @brief Does this result have a description?
     *
     *  This function is used to determine if the developer has provided a
     *  human-readable description of this result field or not.
     *
     *  @return True if this result has a description and false otherwise.
     *
     *  @throw none No throw guarantee.
     */
    bool has_description() const noexcept { return m_desc_.has_value(); }

    /** @brief Sets the type this result field must have.
     *
     *  Result fields are always types akin to std::shared_ptr<const T>. This
     *  function sets the RTII of the field to that of T. T is determined by
     *  the ModuleResult class and fowarded to this class.
     *
     *  @param[in] new_type The RTTI of this field.
     *
     *  @throw std::runtime_error if the value has already been set and this
     *                            call would change the type. Strong throw
     *                            guarantee.
     */
    void set_type(type::rtti new_type);

    /** @brief Binds a value to this result field
     *
     *  This function is used to bind @p new_value to the present field. This
     *  call will ensure that the field is ready (i.e., the type of the field
     *  is set) and that @p new_value is of the proper type.
     *
     * @param[in] new_value The value to bind to this field.
     *
     * @throw std::bad_optional_access if the type has not been set. Strong
     *                                 throw guarantee.
     * @throw std::invalid_argument if @p new_value does not have the correct
     *                              type. Strong throw guarantee.
     */
    void set_value(shared_any new_value);

    /** @brief Sets this result field's description.
     *
     *  This function is used to set the human-readable description of what this
     *  result field is. If the description has been previously set this call
     *  will overwrite the previous description.
     *
     *  @param[in] desc The description of this result field.
     *
     *  @throw none No throw guarantee.
     */
    void set_description(type::description desc) noexcept;

    /** @brief Returns the RTTI associated with the bound value
     *
     *  Each field must be of a well defined type. This function is used to
     *  retrieve the RTTI of that type.
     *
     *  @return The RTTI of the bound value.
     *
     *  @throw std::bad_optional_access if the type of this field has not been
     *                                  set. Strong throw guarantee.
     */
    type::rtti type() const { return m_type_.value(); }

    /** @brief Retrieves the bound value.
     *
     *  This function can be used to retrieve the value bound to this result
     *  field. If no value is currently bound then an error will be raised.
     *
     *  @return A shared pointer to the type-erased bound value.
     *
     *  @throw std::runtime_error if no value is bound to this field. Strong
     *                            throw guarantee.
     */
    auto& value() const;

    /** @brief Retrieves the human-readable description of the bound value.
     *
     *  Developers are encouraged to write descriptions of what a result
     *  represents. These strings are purely for edification. This function
     *  can be used to retrieve the description, if the developer wrote one.
     *
     *  @return The human-readable description of the field.
     *
     *  @throw std::bad_optional_access if this field does not have a
     *                                  description. Strong throw guarantee.
     */
    auto& description() const { return m_desc_.value(); }

    /** @brief Compares two ModuleResultPIMPL instances for equality
     *
     *  Two ModuleResultPIMPL instances are equivalent if they hold the same
     *  value, have the same description of that value, and require the same
     *  type.
     *
     *  @param[in] rhs The instance to compare to.
     *
     *  @return True if the two instances are equal and false otherwise.
     *
     *  @throw ??? Throws if the value's comparison throws. Same guarantee.
     */
    bool operator==(const ModuleResultPIMPL& rhs) const;

    /** @brief Determines if two ModuleResultPIMPL instances are different
     *
     *  Two ModuleResultPIMPL instances are equivalent if they hold the same
     *  value, have the same description of that value, and require the same
     *  type.
     *
     *  @param[in] rhs The instance to compare to.
     *
     *  @return False if the two instances are equal and true otherwise.
     *
     *  @throw ??? Throws if the value's comparison throws. Same guarantee.
     */
    bool operator!=(const ModuleResultPIMPL& rhs) const;

    /** @brief Enables serialization for ModuleResultPIMPL instances.
     *
     * This function saves the ModuleResultPIMPL instance into an Archive
     * object.
     *
     * @tparam Archive The type of archive used for serialization.
     */
    void serialize(Serializer& s) const;

    /** @brief Enables deserialization for ModuleResultPIMPL instances.
     *
     * This function loads the ModuleResultPIMPL instance from the Archive
     * object.
     *
     * @tparam Archive The type of archive used for deserialization.
     * instance.
     */
    void deserialize(Deserializer& d);

private:
    /// The type-erased value bound to this field
    shared_any m_value_;

    /// A human-readable description of what this field is
    std::optional<type::description> m_desc_;

    /// The RTTI of this field
    std::optional<type::rtti> m_type_;

}; // class ModuleResultPIMPL

//-----------------------------------Implementations----------------------------

inline void ModuleResultPIMPL::set_type(type::rtti new_type) {
    if(has_value() && type() != new_type)
        throw std::runtime_error("Can't change type after value is set");
    m_type_.emplace(std::type_index(new_type));
}

inline void ModuleResultPIMPL::set_value(shared_any new_value) {
    if(std::type_index(new_value->type()) != type()) {
        std::string msg{"Value is expected to be of type: "};
        msg += utilities::printing::Demangler::demangle(type().name());
        msg += ", but the given set_value type is:";
        msg +=
          utilities::printing::Demangler::demangle(new_value->type().name());

        throw std::invalid_argument(msg);
    }
    m_value_ = new_value;
}

inline void ModuleResultPIMPL::set_description(
  type::description desc) noexcept {
    m_desc_.emplace(std::move(desc));
}

inline auto& ModuleResultPIMPL::value() const {
    if(has_value()) return m_value_;
    throw std::runtime_error("Result does not have a bound value");
}

inline bool ModuleResultPIMPL::operator==(const ModuleResultPIMPL& rhs) const {
    if(has_type() != rhs.has_type()) return false;
    if(has_value() != rhs.has_value()) return false;
    if(has_description() != rhs.has_description()) return false;

    if(has_type() && type() != rhs.type()) return false;
    if(has_value() && (*value() != *(rhs.value()))) return false;
    if(has_description() && description() != rhs.description()) return false;

    return true;
}

inline bool ModuleResultPIMPL::operator!=(const ModuleResultPIMPL& rhs) const {
    return !((*this) == rhs);
}

inline void ModuleResultPIMPL::serialize(Serializer& s) const {
    s(cereal::make_nvp("ModuleResultPIMPL has_description", has_description()));
    if(has_description())
        s(cereal::make_nvp("ModuleResultPIMPL description", m_desc_));
    s(cereal::make_nvp("ModuleResultPIMPL has_type", has_type()));
    if(has_type()) {
        s(cereal::make_nvp("ModuleResultPIMPL has_value", has_value()));
        if(has_value()) {
            s(cereal::make_nvp("ModuleResultPIMPL value", m_value_));
        } else {
            std::cout << "Warning! Type cannot be serialized." << std::endl;
        }
    }
}

inline void ModuleResultPIMPL::deserialize(Deserializer& d) {
    bool hasdescription, hastype, hasvalue;
    d(cereal::make_nvp("ModuleResultPIMPL has_description", hasdescription));
    if(hasdescription)
        d(cereal::make_nvp("ModuleResultPIMPL description", m_desc_));
    d(cereal::make_nvp("ModuleResultPIMPL has_type", hastype));
    if(hastype) {
        d(cereal::make_nvp("ModuleResultPIMPL has_value", hasvalue));
        if(hasvalue) {
            d(cereal::make_nvp("ModuleResultPIMPL value", m_value_));
            m_type_.emplace(std::type_index(m_value_.get()->type()));
        }
    }
}

} // namespace sde::detail_
