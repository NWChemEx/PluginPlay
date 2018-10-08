set(
        SDE_SRCS Module.cpp
                 ModuleManager.cpp
)
set(
        SDE_INCLUDES Memoization.hpp
		     Option.hpp
                     Pythonization.hpp
                     Parameters.hpp
                     SDEAny.hpp
                     Serialization.hpp
                     ModuleHelpers.hpp
                     Module.hpp
                     ModuleManager.hpp
)

if(BUILD_PYBINDINGS)
    list(   APPEND SDE_SRCS PyBindings/ExportSDE.cpp
                            PyBindings/PyParameters.cpp
                            PyBindings/PyModule.cpp
                            PyBindings/PyModuleManager.cpp
    )
    list(
            APPEND SDE_INCLUDES PyBindings/PyModule.hpp
            PyBindings/PySmartEnums.hpp
    )
    add_python_module(
            NAME SDE
            SOURCES ${SDE_SRCS}
            DEPENDS bphash::bphash cereal::cereal
    )
    target_compile_definitions(SDE PUBLIC USING_pybind11)
    if(APPLE)
        set_target_properties(SDE PROPERTIES SUFFIX .so)
    endif()
else()
    add_nwx_library(
            NAME SDE
            SOURCES ${SDE_SRCS}
            DEPENDS bphash::bphash cereal::cereal
    )
endif()
install_targets(TARGETS SDE INCLUDES ${SDE_INCLUDES})
