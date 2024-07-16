cmake_minimum_required(VERSION 3.28.1)

project(repl-sql-autocomplete VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Catch REQUIRED)

set(CXX_COMMON_FLAGS "-Wall -Wextra -Werror")
set(CXX_COMMON_FLAGS "${CXX_COMMON_FLAGS} -Weffc++ -Wsign-compare -Wshadow")
set(CXX_COMMON_FLAGS "${CXX_COMMON_FLAGS} -Wwrite-strings -Wpointer-arith")
set(CXX_COMMON_FLAGS "${CXX_COMMON_FLAGS} -Winit-self -Wconversion")
set(CXX_COMMON_FLAGS "${CXX_COMMON_FLAGS} -Wno-sign-conversion -Wundefined-inline")
set(CMAKE_CXX_FLAGS ${CXX_COMMON_FLAGS})

set(PROJECT_ARTIFACT_APP ${PROJECT_NAME})
set(PROJECT_ARTIFACT_TEST "${PROJECT_NAME}-test")

include_directories(src)

file(GLOB_RECURSE SRC CONFIGURE_DEPENDS src/*.hpp src/*.cpp)
add_executable(${PROJECT_ARTIFACT_APP} ${SRC})

file(GLOB_RECURSE TEST CONFIGURE_DEPENDS src/*.hpp src/*/*.cpp test/*.hpp test/*.cpp)
add_executable(${PROJECT_ARTIFACT_TEST} ${TEST})
target_link_libraries(${PROJECT_ARTIFACT_TEST} PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(${PROJECT_ARTIFACT_TEST})
