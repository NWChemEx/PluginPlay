list(APPEND sde_TESTS  printing/detail_/print_inputs.cpp
                       printing/detail_/print_results.cpp
                       printing/detail_/print_submodules.cpp
                       printing/detail_/rest_printer.cpp
                       printing/document_module.cpp
)

set(sde_TESTS "${sde_TESTS}" PARENT_SCOPE)
