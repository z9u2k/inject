include_directories(${INJECT_SOURCE_DIR}/src)
add_executable(simple_component main.cpp service.cpp service_impl.cpp context_config.cpp)
