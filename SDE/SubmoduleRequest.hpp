#pragma once
#include "SDE/Module.hpp"
#include <memory>
#include <string>

namespace SDE {
namespace detail_ {
class SubmoduleRequestPIMPL;
}

class SubmoduleRequest {
public:
    using description_type = std::string;
    using module_type      = Module;
    using module_ptr       = std::shared_ptr<const Module>;

    SubmoduleRequest();
    SubmoduleRequest(const SubmoduleRequest& rhs);
    SubmoduleRequest& operator=(const SubmoduleRequest&);
    SubmoduleRequest(SubmoduleRequest&& rhs) noexcept;
    SubmoduleRequest& operator=(SubmoduleRequest&& rhs) noexcept;
    ~SubmoduleRequest() noexcept;

    template<typename property_type, typename... Args>
    auto run_as(Args&&... args) {
        if(!check_type_(typeid(property_type)))
            throw std::invalid_argument("Wrong property type");
        return value().run_as<property_type>(std::forward<Args>(args)...);
    }

    const module_type& value() const;

    const description_type& description() const noexcept;
    void hash(bphash::Hasher& h) const { value().hash(h); }
    void change(module_ptr new_module);

    template<typename T>
    auto& set_type() noexcept {
        set_type_(typeid(std::decay_t<T>));
        return *this;
    }

    SubmoduleRequest& set_description(description_type desc) noexcept;

private:
    bool check_type_(const std::type_info& type) const noexcept;
    void set_type_(const std::type_info& type) noexcept;

    std::unique_ptr<detail_::SubmoduleRequestPIMPL> pimpl_;
};

} // namespace SDE
