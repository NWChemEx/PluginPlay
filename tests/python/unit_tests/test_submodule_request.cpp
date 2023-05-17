#include "modules.hpp"
#include "property_types.hpp"
#include "test_pluginplay.hpp"
#include <pluginplay/submodule_request.hpp>

namespace test_pluginplay {

void test_submodule_request(pybind11::module_& m) {
    auto m_test = m.def_submodule("test_submodule_request");

    m_test.def("get_sr_w_not_ready_mod", []() {
        pluginplay::SubmoduleRequest sr;
        sr.set_type<OneIn>();
        sr.change(make_module<NotReadyModule2>());
        return sr;
    });

    m_test.def("get_sr_w_ready_mod", []() {
        pluginplay::SubmoduleRequest sr;
        sr.set_type<OptionalInput>();
        sr.change(make_module<ReadyModule>());
        return sr;
    });
}

} // namespace test_pluginplay
