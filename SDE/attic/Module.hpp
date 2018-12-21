#pragma once
#include "ComputedProperties.hpp"
#include "Parameters.hpp"
#include <Utilities/Containers/CaseInsensitiveMap.hpp>

namespace SDE {
namespace detail_ {

} // namespace detail_


class Module{
public:
    using submodule_map = Utilities::CaseInsensitiveMap<Module>;

    Parameters& params();
    const Parameters& params() const {
        return const_cast<Module &>(*this).params();
    }


    /**
     *
     * The `run_as` function is the main API for running a module.
     *
     * @note semantically `mod.run_as<PropertyType>(args...)` is identical to
     * ```
     * PropertyType pt;
     * pt.run(mod, args...);
     * ```
     *
     * @tparam PropertyType
     * @tparam Args
     * @param args
     * @return
     */
    template<typename PropertyType, typename...Args>
    auto run_as(Args&&...args) {
        PropertyType pt;
        return pt.run(*this, std::forward<Args>(args...));
    }

    ComputedProperties run(const Parameters& params);


};

} // namespace SDE

/** @brief Macro to factor out boilerplate for defining a property type
 *
 *  @param[in] prop_name Name to be used for resulting class
 *  @param[in] return_value Type of the return
 *  @param[in] __VA_ARGS__ The types of the inputs to the module.
 */
#define DEFINE_PROPERTY_TYPE(prop_name, return_value, ...) \
    struct prop_name : SDE::ModuleBase {                   \
        virtual return_value run(__VA_ARGS__) = 0;         \
    }
