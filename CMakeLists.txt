cmake_minimum_required(VERSION 3.16)
project(Afore)

set(CMAKE_CXX_STANDARD 20)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    message(STATUS "HEYOOOO")
    include_directories("C:/Users/maxwe/Documents/git/vcpkg/installed/x64-windows/include")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fconcepts -fpermissive")
elseif(CMAKE_CXX_COMPILER_ID MATCHES Clang)
    include_directories("C:/Users/maxwe/Documents/git/vcpkg/installed/x64-windows/include")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++20 -fpermissive")
endif()
include_directories(include)

add_executable(Afore main.cpp)
add_executable(Afore_tuple tuple_test_main.cpp)
target_sources(Afore PUBLIC include)
target_sources(Afore_tuple PUBLIC include)

