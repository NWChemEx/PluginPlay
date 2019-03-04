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

if(BUILD_PYBINDINGS)
    list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
    find_package(Cppyy REQUIRED)
    cppyy_make_python_package(sde NAMESPACE SDE)
endif()

cpp_install(TARGETS sde)
