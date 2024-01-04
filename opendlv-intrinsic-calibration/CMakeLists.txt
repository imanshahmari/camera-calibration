cmake_minimum_required(VERSION 3.2)
project(intrinsic)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

find_package(Threads REQUIRED)
find_package(yaml-cpp REQUIRED)

include_directories(SYSTEM ${CMAKE_BINARY_DIR})

add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/src/intrinsic.cpp)
target_link_libraries(${PROJECT_NAME} Threads::Threads ${OpenCV_LIBS} yaml-cpp)
