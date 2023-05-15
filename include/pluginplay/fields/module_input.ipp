/*
 * Copyright 2022 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// To be included only by module_input.hpp

namespace pluginplay {

inline ModuleInput& ModuleInput::operator=(const ModuleInput& rhs) {
    return *this = std::move(ModuleInput(rhs));
}

template<typename T>
bool ModuleInput::is_valid(T&& new_value) const {
    return is_valid_(wrap_value_(std::forward<T>(new_value)));
}

template<typename T>
T ModuleInput::value() {
    // Did the user ask for the value back by copy?
    constexpr bool by_value = std::is_same_v<std::decay_t<T>, T>;

    // Want a const reference or a copy? Defer to ModuleInput::value() const
    if constexpr(detail_::IsConstRef<T>::value || by_value)
        return const_cast<const ModuleInput&>(*this).value<T>();

    // Know user wants it by read/write reference (okay if we copied the value)

    // Check how it's stored. If not a copy (i.e., it's a const ref) we can't
    // return read/write ref
    if(m_is_cref_)
        throw std::runtime_error("Input is a const ref, so you can't get it "
                                 "back as a modifiable reference.");

    return any::any_cast<T>(get_());
}

template<typename T>
T ModuleInput::value() const {
    const auto& any = get_();
    if constexpr(std::is_same_v<std::decay_t<T>, type::any>) {
        return any;
    } else {
        return any::any_cast<T>(any);
    }
}

template<typename T, typename U>
auto& ModuleInput::add_check(bounds_checking::CompareTo<T, U> check,
                             type::description desc) {
    if(desc.empty()) desc = print_bounds(check);

    validity_check<T> l{[check{std::move(check)}](const T& x) {
        return check(x);
    }}; // namespace pluginplay
    return add_check(std::move(l), desc);
}

template<typename T>
auto& ModuleInput::add_check(bounds_checking::InRange<T> check,
                             type::description desc) {
    if(desc.empty()) desc = print_bounds(check);

    validity_check<T> l{
      [check{std::move(check)}](const T& x) { return check(x); }};
    return add_check(std::move(l), desc);
}

template<typename T>
auto& ModuleInput::add_check(validity_check<T> check, type::description desc) {
    any_check temp = [check{std::move(check)}](const type::any& new_value) {
        return check(any::any_cast<T>(new_value));
    };
    return add_check_(std::move(temp), std::move(desc));
}

inline std::string ModuleInput::str() const {
    std::stringstream ss;
    get_().print(ss);
    return ss.str();
}

template<typename T>
auto& ModuleInput::set_type() {
    // Make sure the user isn't trying to set the input type to read/write
    // reference.
    constexpr bool is_ref   = std::is_reference_v<T>;
    constexpr bool is_c_ref = detail_::IsConstRef<T>::value;
    static_assert(is_c_ref || !is_ref,
                  "T should not be an lvalue or rvalue reference.");

    // Make sure it is not a pointer
    static_assert(!std::is_pointer_v<T>, "T should't be a pointer");

    // We need to clean-up the type for the pluginplayAny, basically we need to
    // get it down to just cv qualifified.
    using no_ref = std::remove_reference_t<T>;

    m_is_cref_ = is_c_ref;
    set_type_(typeid(no_ref));        // Sets type as seen by outside world
    return add_type_check_<no_ref>(); // Set
}

template<typename T>
auto& ModuleInput::change(T&& new_value) {
    // By convention we store strings as std::string, so we have to catch when
    // the user passed a C-string in
    if constexpr(detail_::IsCString<T>::value) {
        change_(std::move(wrap_value_(std::string(new_value))));
    } else {
        change_(std::move(wrap_value_(std::forward<T>(new_value))));
    }
    return *this;
}

template<typename T>
auto& ModuleInput::set_default(T&& new_value) {
    return change(std::forward<T>(new_value));
}

inline bool ModuleInput::operator!=(const ModuleInput& rhs) const noexcept {
    return !((*this) == rhs);
}

inline type::any& ModuleInput::get_() {
    const auto& temp = const_cast<const ModuleInput&>(*this).get_();
    return const_cast<type::any&>(temp);
}

template<typename T>
type::any ModuleInput::wrap_value_(T&& new_value) const {
    using clean_type = std::decay_t<T>;
    using cref_type  = const clean_type&;

    // Did the user give us an AnyField already?
    if constexpr(std::is_same_v<clean_type, type::any>) { return new_value; }

    // Did the user give us a reference (so either T=U& or T=U&&)
    constexpr bool is_ref = std::is_reference_v<T>;

    // N.B. if is_ref is false, then ownership is being transferred to us. So
    // even if we only need a reference we have to take the object by value or
    // we'll end up with lifetime issues.
    if(m_is_cref_ && is_ref)
        return any::make_any_field<cref_type>(std::forward<T>(new_value));
    else
        return any::make_any_field<clean_type>(std::forward<T>(new_value));
}

template<typename T>
auto& ModuleInput::add_type_check_() {
    bounds_checking::TypeCheck<T> check;
    auto l = [=](const type::any& value) { return check(value); };
    return add_check_(l, check.str());
}

} // namespace pluginplay
