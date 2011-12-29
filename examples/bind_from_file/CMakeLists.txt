include_directories(${INJECT_SOURCE_DIR}/src)
add_executable(bind_from_file main.cpp simple_format_config.cpp service.cpp service_impl.cpp)
