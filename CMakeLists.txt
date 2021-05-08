cmake_minimum_required(VERSION 3.16)
project(SCZR_Snake)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -pthread")

find_package(OpenCV REQUIRED)

file(GLOB TARGET_SRC "./src/*.cpp" )
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS)

set(EXECUTABLE_NAME "SCZR_Snake")
include_directories(
    ${OpenCV_INCLUDE_DIRS} 
    ${CMAKE_HOME_DIRECTORY}/include/)

add_library(
        shared_memory_lib
        ./include/shared_memory.hpp
        ./src/shared_memory.cpp)
add_library(
        synchronizer_lib
        ./include/synchronizer.hpp
        ./src/synchronizer.cpp
)

add_executable(capture src/capture.cpp src/CaptureVideo.cpp)
add_executable(process src/process.cpp src/ImageProcessing.cpp src/Snake.cpp)
add_executable(game src/game.cpp)
add_executable(start src/start.cpp)
add_executable(info src/info.cpp src/Log.cpp)

target_link_libraries(shared_memory_lib rt)
target_link_libraries(synchronizer_lib shared_memory_lib)

target_link_libraries(info synchronizer_lib)
target_link_libraries(process synchronizer_lib ${OpenCV_LIBS})
target_link_libraries(capture synchronizer_lib ${OpenCV_LIBS})
target_link_libraries(game synchronizer_lib ${OpenCV_LIBS})
target_link_libraries(start shared_memory_lib rt)