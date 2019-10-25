list(APPEND sde_TESTS
     detail_/lambda_module.cpp
     detail_/module_input_pimpl.cpp
     detail_/module_manager_pimpl.cpp
     detail_/module_pimpl.cpp
     detail_/module_result_pimpl.cpp
     detail_/property_type_builder.cpp
     detail_/sde_any.cpp
     detail_/sde_any_wrapper.cpp
     detail_/submodule_request_pimpl.cpp
)
set(sde_TESTS "${sde_TESTS}" PARENT_SCOPE)
