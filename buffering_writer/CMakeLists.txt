cmake_minimum_required(VERSION 3.10)
project(buffering_writer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(lib_buffering_writer src/buffering_writer.cpp)

add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    lib_buffering_writer
    )
