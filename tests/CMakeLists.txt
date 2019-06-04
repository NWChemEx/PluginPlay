set(sde_TESTS test_main.cpp
              detail_/module_input_pimpl.cpp
              detail_/module_manager_pimpl.cpp
              detail_/module_pimpl.cpp
              detail_/module_result_pimpl.cpp
              detail_/property_type_builder.cpp
              detail_/sde_any.cpp
              detail_/submodule_request_pimpl.cpp
              examples/module_base.cpp
              examples/property_type.cpp
              examples/workflow.cpp
              module.cpp
              module_input.cpp
              module_manager.cpp
              module_result.cpp
              submodule_request.cpp
              utility.cpp
#        TestCache.cpp
#        #	      TestMemoization.cpp
)

cpp_add_executable(
        test_sde
        SOURCES ${sde_TESTS}
        DEPENDS sde Catch2::Catch2
)
target_include_directories(test_sde PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
add_test(NAME test_sde COMMAND test_sde)

if(BUILD_PYBINDINGS)
set(sde_TESTS ${sde_TESTS} module.py
	                   examples/module_base.py
			   examples/property_type.py
			   examples/workflow.py
			   )
foreach(test_name module
		  examples/module_base
		  examples/property_type
		  examples/workflow
		  )
	  add_test(NAME py_${test_name}
	  COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/${test_name}.py
	  )
endforeach()
endif()
