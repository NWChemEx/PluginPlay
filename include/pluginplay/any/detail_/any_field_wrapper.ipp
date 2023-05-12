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

#include <iostream>
#include <pluginplay/python/python_wrapper.hpp>
#include <utilities/printing/print_stl.hpp>

namespace pluginplay::any::detail_ {

#define TEMPLATE_PARAMS template<typename T>
#define ANY_FIELD_WRAPPER AnyFieldWrapper<T>

TEMPLATE_PARAMS template<typename U, typename>
ANY_FIELD_WRAPPER::AnyFieldWrapper(U&& value2wrap) :
  base_type(wrap_value_(std::forward<U>(value2wrap))) {}

TEMPLATE_PARAMS
typename ANY_FIELD_WRAPPER::field_base_pointer ANY_FIELD_WRAPPER::clone_()
  const {
    using deep_copy_type = AnyFieldWrapper<clean_type>;
    if constexpr(std::is_same_v<clean_type, python_value>) {
        return std::make_unique<deep_copy_type>(as_python_wrapper());
    } else {
        const auto& val = base_type::template cast<const_ref_type>();
        return std::make_unique<deep_copy_type>(val);
    }
}

TEMPLATE_PARAMS
bool ANY_FIELD_WRAPPER::are_equal_(const AnyFieldBase& rhs) const noexcept {
    auto prhs = dynamic_cast<const my_type*>(&rhs);
    if(prhs == nullptr) return false; // Not the same type

    if constexpr(std::is_same_v<clean_type, python_value>) {
        return as_python_wrapper() == rhs.as_python_wrapper();
    } else {
        const auto& lhs_value =
          this->base_type::template cast<const_ref_type>();
        const auto& rhs_value =
          prhs->base_type::template cast<const_ref_type>();
        return lhs_value == rhs_value;
    }
}

TEMPLATE_PARAMS
bool ANY_FIELD_WRAPPER::value_equal_(const AnyFieldBase& rhs) const noexcept {
    // Regardless of how it's wrapped, every wrapped value can be converted to
    // a const ref, so we first check rhs can be converted to the same const ref
    if(!rhs.is_convertible<const_ref_type>()) return false;

    const auto& lhs_value = this->base_type::template cast<const_ref_type>();
    const auto& rhs_value = rhs.cast<const_ref_type>();
    return lhs_value == rhs_value;
}

TEMPLATE_PARAMS
std::ostream& ANY_FIELD_WRAPPER::print_(std::ostream& os) const {
    using utilities::printing::operator<<;
    if constexpr(utilities::type_traits::is_printable_v<T>) {
        os << this->base_type::template cast<const T&>();
    } else {
        const auto* pvalue = &(this->base_type::template cast<const T&>());
        os << "<" << typeid(T).name() << " " << pvalue << ">";
    }
    return os;
}

TEMPLATE_PARAMS
typename ANY_FIELD_WRAPPER::python_value ANY_FIELD_WRAPPER::as_python_wrapper_()
  const {
    if(this->storing_python_object()) {
        return this->base_type::template cast<python_value>();
    }
    const auto& my_value = this->base_type::template cast<const_ref_type>();
    return python::make_python_wrapper(my_value);
}

TEMPLATE_PARAMS
bool ANY_FIELD_WRAPPER::storing_const_ref_() const noexcept {
    return wrap_const_ref_v;
}

TEMPLATE_PARAMS
bool ANY_FIELD_WRAPPER::storing_const_value_() const noexcept {
    return wrap_const_val_v;
}

TEMPLATE_PARAMS
bool ANY_FIELD_WRAPPER::storing_python_object_() const noexcept {
    return std::is_same_v<clean_type, python::PythonWrapper>;
}

TEMPLATE_PARAMS
template<typename U>
std::any ANY_FIELD_WRAPPER::wrap_value_(U&& value2wrap) const {
    return std::make_any<wrapped_type>(std::forward<U>(value2wrap));
}

#undef ANY_FIELD_WRAPPER
#undef TEMPLATE_PARAMS

} // namespace pluginplay::any::detail_
