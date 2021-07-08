cmake_minimum_required(VERSION 3.8)
project("SystemProperties" VERSION 0.1)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(SystemProperties STATIC SystemProperties.cpp)