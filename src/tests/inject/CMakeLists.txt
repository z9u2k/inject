include_directories(${INJECT_SOURCE_DIR}/src)

add_executable(unit_tests unit_tests.cpp)
target_link_libraries(unit_tests boost_unit_test_framework boost_test_exec_monitor) 
