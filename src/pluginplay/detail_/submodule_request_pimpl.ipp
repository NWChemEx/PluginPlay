// This file meant only for inclusion in submodule_request_pimpl.hpp

namespace pluginplay::detail_ {

inline auto SubmoduleRequestPIMPL::clone() const {
    return std::make_unique<SubmoduleRequestPIMPL>(*this);
}

inline bool SubmoduleRequestPIMPL::ready() const {
    // Relies on short-circuiting to avoid the segfault
    return has_type() && has_module() && m_module_->ready(m_inputs_);
}

inline void SubmoduleRequestPIMPL::set_type(type::rtti type,
                                            type::input_map inputs) {
    if(has_module() && !value().property_types().count(type))
        throw std::runtime_error("Can't change type after setting module");
    m_type_.emplace(type);
    m_inputs_ = std::move(inputs);
}

inline void SubmoduleRequestPIMPL::set_module(module_ptr ptr) {
    if(!ptr) throw std::runtime_error("Pointer does not contain a module");
    // Type will check that a property type was set
    if(ptr->property_types().count(type()) == 0) {
        std::string msg("Module does not satisfy property type: ");
        msg += utilities::printing::Demangler::demangle(type());
        throw std::runtime_error(msg);
    }
    m_module_ = ptr;
}

inline void SubmoduleRequestPIMPL::set_description(std::string desc) noexcept {
    m_desc_.emplace(std::move(desc));
}

inline const Module& SubmoduleRequestPIMPL::value() const {
    if(!has_module()) throw std::runtime_error("Submodule is not ready");
    return *m_module_;
}

inline Module& SubmoduleRequestPIMPL::value() {
    if(!has_module()) throw std::runtime_error("Submodule is not ready");
    return *m_module_;
}

inline void SubmoduleRequestPIMPL::lock() {
    if(!ready()) throw std::runtime_error("Can't lock a non-ready module");
    m_module_->lock();
}

inline SubmoduleRequestPIMPL::submod_uuid_map
SubmoduleRequestPIMPL::submod_uuids() const {
    if(!has_module()) throw std::runtime_error("Submodule is not set");
    return m_module_->submod_uuids();
}

inline SubmoduleRequestPIMPL::uuid_type SubmoduleRequestPIMPL::uuid() const {
    if(!has_module()) throw std::runtime_error("Submodule is not set");
    return m_module_->uuid();
}

inline bool SubmoduleRequestPIMPL::operator==(
  const SubmoduleRequestPIMPL& rhs) const {
    if(has_type() != rhs.has_type()) return false;
    if(has_description() != rhs.has_description()) return false;
    if(has_module() != rhs.has_module()) return false;

    if(has_type() && type() != rhs.type()) return false;
    if(has_description() && description() != rhs.description()) return false;
    if(has_module() && (value() != rhs.value())) return false;
    return true;
}

inline bool SubmoduleRequestPIMPL::operator!=(
  const SubmoduleRequestPIMPL& rhs) const {
    return !((*this) == rhs);
}

} // namespace pluginplay::detail_
