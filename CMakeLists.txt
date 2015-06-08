cmake_minimum_required(VERSION 2.5)
project(CRF_Bernat)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    Read.cpp)

add_executable(CRF_Bernat ${SOURCE_FILES})
