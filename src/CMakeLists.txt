cmake_minimum_required(VERSION 3.27)

project(Project VERSION 1.0)

add_library(compiler_flags INTERFACE)
target_compile_features(compiler_flags INTERFACE cxx_std_23)

set(METRIC_DIR ../metric)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(${METRIC_DIR} ${CMAKE_BINARY_DIR}/metric_build)

add_executable(Project main.cpp)

target_link_libraries(Project PRIVATE
                      metric
                      compiler_flags
)

target_include_directories(Project PRIVATE
                           ${CMAKE_BINARY_DIR}
                           ${METRIC_DIR}
)