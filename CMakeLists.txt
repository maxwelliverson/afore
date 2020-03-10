cmake_minimum_required(VERSION 3.16)
project(Afore)

set(CMAKE_CXX_STANDARD 20)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fconcepts -fpermissive")
else()
endif()
include_directories(include)

add_executable(Afore main.cpp)
add_executable(Afore_tuple tuple_test_main.cpp)
target_sources(Afore PUBLIC include)
target_sources(Afore_tuple PUBLIC include)

