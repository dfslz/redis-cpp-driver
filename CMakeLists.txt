cmake_minimum_required(VERSION 3.18.0)

set(CMAKE_CXX_STANDARD 17)

project(redis_cpp_driver)

add_compile_options(-O0 -g)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

file(GLOB_RECURSE SRC src/*.cpp)

add_executable(redis_cpp_driver
    main.cpp
    ${SRC}
)

target_link_libraries(redis_cpp_driver glog)

add_subdirectory(ut) # build ut