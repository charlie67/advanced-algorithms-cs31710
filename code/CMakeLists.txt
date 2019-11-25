cmake_minimum_required(VERSION 3.15)
project(code C)

set(CMAKE_C_STANDARD 99)

add_executable(code main.c)
target_link_libraries(code m)