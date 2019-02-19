set(SDE_SRCS Module.cpp
             ModuleInput.cpp
             ModuleManager.cpp
             ModuleResult.cpp
             SubmoduleRequest.cpp
)
set(SDE_INCLUDES detail_/Memoization.hpp
                 detail_/ModuleInputPIMPL.hpp
                 detail_/ModuleManagerPIMPL.hpp
                 detail_/ModulePIMPL.hpp
                 detail_/ModuleResultPIMPL.hpp
                 detail_/PropertyTypeBuilder.hpp
                 detail_/SDEAny.hpp
                 detail_/SubmoduleRequestPIMPL.hpp
                 detail_/Serializer.hpp
                 Module.hpp
                 ModuleBase.hpp
                 ModuleInput.hpp
                 ModuleManager.hpp
                 ModuleResult.hpp
                 PropertyType.hpp
                 SubmoduleRequest.hpp
                 Types.hpp
                 Utility.hpp
)
cpp_add_library(
    sde
    SOURCES "${SDE_SRCS}"
    INCLUDES "${SDE_INCLUDES}"
    DEPENDS utilities::utilities bphash::bphash cereal
)

#set(
#        SRCS attic/ModuleBase.cpp
#        ModuleManager.cpp
#        SubmoduleRequestForm.cpp
#)
#set(
#    INCS detail_/IOMap.hpp
#         Cache.hpp
#       	Memoization.hpp
#         Pythonization.hpp
#         Parameters.hpp
#    detail_/SDEAny.hpp
#         Serialization.hpp
#         ModuleHelpers.hpp
#    attic/Module.hpp
#         ModuleManager.hpp
#         Option.hpp
#         SubmoduleRequestForm.hpp
#)
#
#if(BUILD_PYBINDINGS)
#    cpp_find_or_build_dependency(
#        NAME pybind11
#        URL github.com/pybind/pybind11
#        CMAKE_ARGS PYBIND11_TEST=OFF
#    )
#    list(
#       APPEND SRCS PyBindings/ExportSDE.cpp
#                   PyBindings/PyParameters.cpp
#                   PyBindings/PyModule.cpp
#                   PyBindings/PyModuleManager.cpp
#    )
#    list(
#       APPEND INCS PyBindings/PyModule.hpp
#                   PyBindings/PySmartEnums.hpp
#    )
#    cpp_add_library(
#        sde
#        SOURCES ${SRCS}
#        INCLUDES ${INCS}
#        DEPENDS cereal utilities::utilities bphash::bphash
#                pybind11::pybind11 pybind11::embed
#    )
#    target_compile_definitions(sde PUBLIC USING_pybind11)
#    if(APPLE)
#        set_target_properties(sde PROPERTIES SUFFIX .so)
#    endif()
#else()
#    cpp_add_library(
#        sde
#        SOURCES ${SDE_SRCS}
#        INCLUDES ${SDE_INCLUDES}
#        DEPENDS cereal utilities::utilities bphash::bphash
#    )
#endif()
cpp_install(TARGETS sde)
