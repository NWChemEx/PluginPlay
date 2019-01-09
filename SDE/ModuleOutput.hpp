#pragma once
#include "SDE/detail_/SDEAny.hpp"
#include <memory>
#include <string>

namespace SDE {
namespace detail_ {
class ModuleOutputPIMPL;
}

class ModuleOutput {
private:
    template<typename T>
    struct IsSharedPtr : std::false_type {};
    template<typename T>
    struct IsSharedPtr<std::shared_ptr<T>> : std::true_type {};

public:
    using description_type = std::string;
    using any_type         = detail_::SDEAny;
    using shared_any       = std::shared_ptr<const any_type>;

    ModuleOutput();
    ModuleOutput(const ModuleOutput& rhs);
    ModuleOutput& operator=(const ModuleOutput& rhs);
    ModuleOutput(ModuleOutput&& rhs) noexcept;
    ModuleOutput& operator=(ModuleOutput&& rhs) noexcept;
    ~ModuleOutput() noexcept;

    template<typename T>
    T value() const {
        using clean_T = std::decay_t<T>;
        if constexpr(std::is_same_v<shared_any, clean_T>)
            return at_();
        else if constexpr(IsSharedPtr<clean_T>::value) {
            using type = typename clean_T::element_type;
            return T(at_(), &value<type&>());
        } else
            return detail_::SDEAnyCast<T>(*at_());
    }

    const description_type& description() const noexcept;

    template<typename T>
    void change(T&& new_value) {
        using clean_T = std::decay_t<T>;
        constexpr bool is_shared_any =
          std::is_same_v<clean_T, shared_any> || // is shared_ptr<const any>
          std::is_same_v<clean_T, std::shared_ptr<any_type>>; // no const
        if constexpr(is_shared_any)
            change_(std::forward<T>(new_value));
        else
            change_(std::move(wrap_value_(std::forward<T>(new_value))));
    }

    template<typename T>
    auto& set_type() {
        constexpr bool is_clean = std::is_same_v<std::decay_t<T>, T>;
        static_assert(is_clean, "Outputs must be unqualified types.");
        return set_type_(typeid(T));
    }

    ModuleOutput& set_description(description_type desc) noexcept;

private:
    template<typename T>
    static any_type wrap_value_(T&& new_value) {
        using clean_T = std::decay_t<T>;
        return detail_::make_SDEAny<clean_T>(std::forward<T>(new_value));
    }

    const shared_any& at_() const noexcept;

    void change_(any_type new_value);
    void change_(shared_any new_value) noexcept;

    ModuleOutput& set_type_(const std::type_info& type) noexcept;

    std::unique_ptr<detail_::ModuleOutputPIMPL> pimpl_;
};

} // namespace SDE
