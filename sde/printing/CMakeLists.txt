list(APPEND SDE_SRCS printing/detail_/print_inputs.cpp
                     printing/detail_/print_results.cpp
                     printing/detail_/rest_printer.cpp
                     printing/detail_/print_submodules.cpp
                     printing/document_module.cpp
)
set(SDE_SRCS "${SDE_SRCS}" PARENT_SCOPE)
