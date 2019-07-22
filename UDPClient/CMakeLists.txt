cmake_minimum_required(VERSION 3.5.1)
project(UDPClient)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -pthread")

find_package( Boost REQUIRED COMPONENTS system)

include_directories(${Boost_INCLUDE_DIRS})

FILE(GLOB SOURCE_FILES *.cpp)
FILE(GLOB INC_FILES *.h)

add_executable(client ${INC_FILES} ${SOURCE_FILES})

target_link_libraries(client "${Boost_LIBRARIES}")
