add_library(metric SHARED event.cpp metric.cpp)
target_include_directories(metric PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)