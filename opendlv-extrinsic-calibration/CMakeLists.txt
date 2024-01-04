cmake_minimum_required(VERSION 3.2)
project(extrinsic)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

find_package(Threads REQUIRED)
find_package(yaml-cpp REQUIRED)

set(INPUT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src/opendlv.odvd)
set(OUTPUT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src/messages.hpp)

add_custom_command(
    OUTPUT ${OUTPUT_FILE}
    COMMAND cluon-msc --cpp --out=${OUTPUT_FILE} ${INPUT_FILE}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS ${INPUT_FILE}
)

# Include the generated header file directory
include_directories(SYSTEM ${CMAKE_BINARY_DIR})


add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/messages.hpp)
target_link_libraries(${PROJECT_NAME} Threads::Threads ${OpenCV_LIBS} yaml-cpp)
