cmake_minimum_required(VERSION 3.10)

project(MyBoundedQueue)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(lib_bounded_queue src/bounded_queue.cpp)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE lib_bounded_queue)
