#pragma once

namespace pluginplay::detail_ {

/** @brief Struct that provides clearer error message when types are not
 * convertible.
 *
 * This allows the replacement of a static assert of the form:
 * `static_assert(std::is_convertible_v<T1,T2>)`
 * with instantiation of a templated struct as:
 * `STATIC_ASSERT_CONVERTIBLE_VERBOSE<T1, T2, I>()`
 * where `I` is intended to be used as an index describing the position of the
 * argument that should have type `T2`
 *
 * The only advantage of using this over the simple `static_assert` is that it
 * will give a slightly simpler error message showing a smaller set of template
 * parameters.
 *
 * @tparam T1 The type of the object to convert from
 * @tparam T2 The type of the object to convert to
 * @tparam I position of the argument in some argument list
 */
template<typename T1, typename T2, std::size_t I>
struct STATIC_ASSERT_CONVERTIBLE_VERBOSE {
    static_assert(std::is_convertible_v<T1, T2>,
                  R"(

    Incorrect argument type; type T1 not convertible to type T2 at position I.
    For values of T1, T2, and I, see compiler output formatted as:
        struct pluginplay::detail_::STATIC_ASSERT_CONVERTIBLE_VERBOSE<T1, T2, I>
        )");
};

} // namespace pluginplay::detail_