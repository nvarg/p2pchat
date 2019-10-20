cmake_minimum_required(VERSION 2.6)
project(p2pchat)

#set(CMAKE_CXX_COMPILER "/usr/bin/clang++" CACHE string "clang compiler" FORCE)
#set(CMAKE_CXX_FLAGS "-std=c++2a -O3 -march=native -DNDEBUG -fno-exceptions -fcoroutines-ts -stdlib=libc++")

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS system REQUIRED)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)

include_directories(${Boost_INCLUDE_DIR})
include_directories("${PROJECT_INCLUDE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}")


set(PROJECT_SOURCES
    ${PROJECT_SOURCE_DIR}/chat.cpp
    ${PROJECT_SOURCE_DIR}/client.cpp
    ${PROJECT_SOURCE_DIR}/listener.cpp
    ${PROJECT_SOURCE_DIR}/main.cpp
)

add_executable(p2pchat ${PROJECT_SOURCES})
target_link_libraries(p2pchat ${Boost_LIBRARIES})
#target_link_libraries(p2pchat PRIVATE fmt::fmt-header-only)
