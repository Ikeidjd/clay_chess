cmake_minimum_required(VERSION 3.10)

add_subdirectory(third_party/raylib)

project(clay_test VERSION 1.0)

file(GLOB_RECURSE SRC_FILES src/*.c)
add_executable(clay_test ${SRC_FILES})

target_link_libraries(clay_test PRIVATE raylib)

target_include_directories(clay_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(clay_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/third_party)
target_include_directories(clay_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/third_party/clay)
