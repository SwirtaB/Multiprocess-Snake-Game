cmake_minimum_required(VERSION 3.16)
project(SCZR_Snake)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(OpenCV REQUIRED)

file(GLOB TARGET_SRC "./src/*.cpp" )
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS)

set(EXECUTABLE_NAME "SCZR_Snake")
include_directories(${CMAKE_HOME_DIRECTORY}/include ${OpenCV_INCLUDE_DIRS} )
add_executable(${EXECUTABLE_NAME} ${TARGET_SRC})

target_link_libraries(${EXECUTABLE_NAME} ${OpenCV_LIBS})
