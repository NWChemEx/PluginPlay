set(SDE_SRCS module.cpp
             module_input.cpp
             module_manager.cpp
             module_result.cpp
             submodule_request.cpp
)
set(SDE_INCLUDES detail_/memoization.hpp
                 detail_/module_input_pimpl.hpp
                 detail_/module_manager_pimpl.hpp
                 detail_/module_pimpl.hpp
                 detail_/module_result_pimpl.hpp
                 detail_/property_type_builder.hpp
                 detail_/sde_any.hpp
                 detail_/sde_any_wrapper.hpp
                 detail_/submodule_request_pimpl.hpp
                 bounds_checking.hpp
                 module.hpp
                 module_base.hpp
                 module_input.hpp
                 module_manager.hpp
                 module_result.hpp
                 property_type.hpp
                 submodule_request.hpp
                 types.hpp
                 utility.hpp
)
cpp_add_library(
    sde
    SOURCES "${SDE_SRCS}"
    INCLUDES "${SDE_INCLUDES}"
    DEPENDS utilities::utilities bphash::bphash cereal
)

cpp_install(TARGETS sde)
