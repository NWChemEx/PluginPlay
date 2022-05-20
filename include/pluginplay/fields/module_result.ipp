// This file meant only for inclusion from module_result.hpp

namespace pluginplay {

//------------------------------Implementations---------------------------------

inline ModuleResult& ModuleResult::operator=(const ModuleResult& rhs) {
    return *this = std::move(ModuleResult(rhs));
}

template<typename T>
auto& ModuleResult::set_type() {
    constexpr bool is_clean = std::is_same_v<std::decay_t<T>, T>;
    static_assert(is_clean, "Results must be unqualified types.");
    return set_type_(typeid(T));
}

template<typename T>
void ModuleResult::change(T&& new_value) {
    using clean_T = std::decay_t<T>;
    constexpr bool is_shared_any =
      std::is_same_v<clean_T, shared_any> || // is shared_ptr<const any>
      std::is_same_v<clean_T, std::shared_ptr<type::any>>; // no const
    if constexpr(is_shared_any)
        change_(new_value);
    else
        change_(std::move(wrap_value_(std::forward<T>(new_value))));
}

template<typename T>
T ModuleResult::value() const {
    using clean_T = std::decay_t<T>;
    if constexpr(std::is_same_v<shared_any, clean_T>)
        return at_();
    else if constexpr(detail_::IsSharedPtr<clean_T>::value) {
        using type = typename clean_T::element_type;
        return T(at_(), &value<type&>());
    } else
        return any::any_cast<T>(*at_());
}

template<typename T>
type::any ModuleResult::wrap_value_(T&& new_value) {
    using clean_T = std::decay_t<T>;
    return any::make_any_field<clean_T>(std::forward<T>(new_value));
}

} // namespace pluginplay