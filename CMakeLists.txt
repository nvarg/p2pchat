cmake_minimum_required(VERSION 2.6)
project(p2pchat)

set(CMAKE_CXX_COMPILER "/usr/bin/clang++" CACHE STRING "clang compiler" FORCE)
set(CMAKE_CXX_FLAGS "-std=c++1z -O3 -fcoroutines-ts -stdlib=libc++")

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS system REQUIRED)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)

include_directories("${Boost_INCLUDE_DIR}")
include_directories("${PROJECT_INCLUDE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}")


set(PROJECT_SOURCES
    ${PROJECT_SOURCE_DIR}/chat.cpp
    ${PROJECT_SOURCE_DIR}/peer.cpp
    ${PROJECT_SOURCE_DIR}/myip.hpp
    ${PROJECT_SOURCE_DIR}/main.cpp
)

add_executable(p2pchat ${PROJECT_SOURCES})
target_link_libraries(p2pchat pthread)
target_link_libraries(p2pchat ${Boost_LIBRARIES})
