cmake_minimum_required(VERSION 3.18.0)

project(RedisCppUT)

file(GLOB_RECURSE UT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*)
file(GLOB_RECURSE DRIVER_SRC ${CMAKE_CURRENT_SOURCE_DIR}/../src/*)

add_executable(ut ut_main.cpp ${UT_SRC} ${DRIVER_SRC})
target_link_libraries(ut gtest.a glog)