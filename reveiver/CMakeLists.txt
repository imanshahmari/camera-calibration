cmake_minimum_required(VERSION 3.2)
project(receiver)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

find_package(Threads REQUIRED)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/messages.hpp
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	COMMAND cluon-msc --cpp --out=${CMAKE_BINARY_DIR}/messages.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/opendlv.odvd
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/opendlv.odvd)

include_directories(SYSTEM ${CMAKE_BINARY_DIR})


add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/receiver.cpp
${CMAKE_BINARY_DIR}/messages.hpp)
target_link_libraries(${PROJECT_NAME} Threads::Threads ${OpenCV_LIBS})
