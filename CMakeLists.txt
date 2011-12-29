cmake_minimum_required(VERSION 2.6)
project(INJECT)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -ansi -pedantic -std=c++98 -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_DEBUG} -Wall -ansi -pedantic -std=c++98 -O2 -DNDEBUG")

FIND_PACKAGE(Boost)
IF (Boost_FOUND)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
    LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
    ADD_DEFINITIONS("-DHAS_BOOST")
ENDIF()

add_subdirectory(src)
add_subdirectory(examples)
