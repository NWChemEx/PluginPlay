#pragma once

namespace sde::detail_ {

/** @brief Class allowing the user to create one-off modules
 *
 */
class LambdaModule : public ModuleBase {
public:
private:
    template<std::size_t i>
    static auto stringify_() {
        return std::to_string(i);
    }
    type::result_map run_(type::input_map inputs,
                          type::submodule_map submods) const;

    std::function<type::result_map(type::input_map, type::submodule_map)>
      m_fxn_;
};

//------------------------------Implementations---------------------------------
template<typename PropertyType>
LambdaModule::LambdaModule(std::function<std::tuple<Returns...>(Args)> fxn,
                           std::index_sequence<Idxs...>)
  m_fxn_([](type::input_map inputs, type::submodule_map submods) {
      auto results = fxn(inputs.at(stringify_<Idx>()).value<Args>()...);

  }){add_input<Args>(stringify_<Idx>())}

type::result_map LambdaModule::run_(type::input_map inputs,
                                    type::submodule_map submods) const {
    return m_fxn_(std::move(inputs), std::move(submods));
}

} // namespace sde::detail_
