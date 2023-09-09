cmake_minimum_required(VERSION 3.18.0)

set(CMAKE_CXX_STANDARD 17)

project(redis_cpp_driver)

add_executable(redis_cpp_driver
        main.cpp
        logger/logger.h
)

target_link_libraries(redis_cpp_driver glog)