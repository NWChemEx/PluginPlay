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
#include <utility>

namespace pluginplay::any::detail_ {

template<typename T>
T AnyFieldBase::cast() {
    // N.B. This relies on calling the non-const version of is_convertible
    if(!is_convertible<T>()) error_if_not_convertible_<T>();
    using clean_type = std::decay_t<T>;

    if(storing_python_object()) {
        auto& py_obj = std::any_cast<python_reference>(m_value_);
        return py_obj.unwrap<T>();
    }

    if constexpr(std::is_same_v<T, python_value>) {
        return as_python_wrapper();
    }

    using mutable_ref             = clean_type&;
    constexpr bool by_mutable_ref = std::is_same_v<T, mutable_ref>;

    if constexpr(!by_mutable_ref) {
        if(storing_const_reference()) {
            // This is the only possible way we use a reference wrapper
            using ref_type = std::reference_wrapper<const clean_type>;
            return std::any_cast<ref_type>(m_value_).get();
        }
    } // else: assert_convertible will catch by_mutable_ref and
      // storing_const_ref

    return std::any_cast<T>(m_value_);
}

template<typename T>
T AnyFieldBase::cast() const {
    // N.B. This relies on calling the const-version of is_convertible
    if(!is_convertible<T>()) error_if_not_convertible_<T>();
    using clean_type = std::decay_t<T>;

    if(storing_python_object()) {
        const auto& py_obj = std::any_cast<const_python_reference>(m_value_);
        return py_obj.unwrap<T>();
    }

    if constexpr(std::is_same_v<T, python_value>) {
        return as_python_wrapper();
    }

    using mutable_ref             = clean_type&;
    constexpr bool by_mutable_ref = std::is_same_v<T, mutable_ref>;
    static_assert(!by_mutable_ref, "Read-only can't be retrieved mutably");

    if(storing_const_reference()) {
        using ref_type = std::reference_wrapper<const clean_type>;
        return std::any_cast<ref_type>(m_value_).get();
    }
    return std::any_cast<T>(m_value_);
}

template<typename T>
bool AnyFieldBase::is_convertible() noexcept {
    using clean_type = std::decay_t<T>;

    // Only differs from non-const overload if we hold a mutable C++ value
    if(storing_const_reference() || storing_const_value() ||
       storing_python_object())
        return std::as_const(*this).is_convertible<T>();

    // Is the user trying to convert it to a PythonWrapper? N.B. since we know
    // it's not a PythonWrapper already we can only return it by value.
    if constexpr(std::is_same_v<T, python_value>) {
        return true;
    } else if constexpr(std::is_same_v<clean_type, python_value>) {
        return false;
    }

    // Getting here means it's stored by mutable value and we can return it
    // however the user wants (as long as the object's actually that type...)
    return std::any_cast<clean_type>(&m_value_) != nullptr;
}

template<typename T>
bool AnyFieldBase::is_convertible() const noexcept {
    using clean_type = std::decay_t<T>;

    // Trying to convert a Python object to a C++ object
    if(storing_python_object()) {
        const auto& py_obj = std::any_cast<const_python_reference>(m_value_);
        return py_obj.is_convertible<T>();
    }

    // Trying to convert to a PythonWrapper? N.B. Since it's not a PythonWrapper
    // already we can only return it by value.
    if constexpr(std::is_same_v<T, python_value>) {
        return true;
    } else if constexpr(std::is_same_v<clean_type, python_value>) {
        return false;
    }

    // C++ to C++ past this point

    // Value is read-only so can only be one of these:
    constexpr bool by_val  = std::is_same_v<clean_type, T>;
    constexpr bool by_cval = std::is_same_v<const clean_type, T>;
    constexpr bool by_cref = std::is_same_v<const clean_type&, T>;

    if constexpr(by_val || by_cval || by_cref) {
        // If stored by cref we need to undo the fact it's in a ref wrapper
        // before comparing
        if(storing_const_reference()) {
            using ref_type = std::reference_wrapper<const clean_type>;
            return std::any_cast<ref_type>(&m_value_) != nullptr;
        }
        // Otherwise just compare them
        return std::any_cast<clean_type>(&m_value_) != nullptr;
    } else {
        return false;
    }
}

template<typename T>
void AnyFieldBase::error_if_not_convertible_() const {
    std::string msg = "Casting to type: ";
    msg += typeid(T).name();
    msg += " failed. AnyField contains an object of type: ";
    msg += type().name();
    msg += ".";
    throw std::runtime_error(msg);
}

} // namespace pluginplay::any::detail_
