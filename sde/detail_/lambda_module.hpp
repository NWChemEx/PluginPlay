#pragma once

namespace utilities {

/// Type trait for when @p T is not an std::tuple or std::pair has  value=false
template<typename T>
struct IsTuple : std::false_type {};

/// Type trait for when @p T is an std::tuple, has value = true
template<typename... Args>
struct IsTuple<std::tuple<Args...>> : std::true_type {};

/// Type trait for when @p T is an std::pair, has value = true
template<typename T, typename U>
struct IsTuple<std::pair<T, U>> : std::true_type {};

/// Convenience global variable for easily getting the result of IsTuple
template<typename T>
static constexpr bool is_tuple_v = IsTuple<T>::value;

} // namespace utilities

namespace sde {
namespace detail_ {

template<typename PropertyType>
class LambdaModule : public ModuleBase {
public:
    template<typename FxnType>
    LambdaModule(FxnType&& fxn);
    LambdaModule(const LambdaModule&) = delete;
    LambdaModule(LambdaModule&&)      = delete;

    type::result_map run_(type::input_map inputs,
                          type::submodule_map submods) const override {
        return m_fxn_(std::move(inputs), std::move(submods));
    }

private:
    std::function<type::result_map(type::input_map, type::submodule_map)>
      m_fxn_;
}; // class LambdaModule
} // namespace detail_

template<typename PropertyType, typename FxnType>
auto make_lambda(FxnType&& fxn);

// ---------------------------Implementations -------------------------------
namespace detail_ {
#define LAMBDA_MOD_TYPE LambdaModule<PropertyType>

template<typename PropertyType>
template<typename FxnType>
LAMBDA_MOD_TYPE::LambdaModule(FxnType&& fxn) :
  ModuleBase(this),
  m_fxn_([&, da_fxn = std::forward<FxnType>(fxn)](type::input_map inputs,
                                                  type::submodule_map)
           ->type::result_map {
               auto unwrapped_inputs = PropertyType::unwrap_inputs(inputs);
               auto rv     = std::apply(da_fxn, std::move(unwrapped_inputs));
               auto result = this->results();
               if constexpr(utilities::is_tuple_v<decltype(rv)>) {
                   auto rv2 = std::tuple_cat(std::make_tuple(std::move(result)),
                                             std::move(rv));
                   result =
                     std::apply(PropertyType::wrap_results, std::move(rv2));
               } else {
                   result = PropertyType::wrap_results(result, std::move(rv));
               }
               return result;
           }) {
    satisfies_property_type<PropertyType>();
}

#undef LAMBDA_MOD_TYPE

} // namespace detail_

template<typename PropertyType, typename FxnType>
auto make_lambda(FxnType&& fxn) {
    using lambda_type = detail_::LambdaModule<PropertyType>;
    auto l = std::make_unique<lambda_type>(std::forward<FxnType>(fxn));
    return Module(std::move(l));
}

} // namespace sde
