cmake_minimum_required(VERSION 3.8)
project("SystemProperties" VERSION 0.1)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(SystemProperties STATIC SystemProperties.hpp SystemProperties.cpp)
target_include_directories(SystemProperties PUBLIC ${CMAKE_CURRENT_LIST_DIR})