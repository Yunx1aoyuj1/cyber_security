cmake_minimum_required(VERSION 3.17)
project(untitled C)

set(CMAKE_C_STANDARD 11)

link_libraries(ws2_32)

add_executable(untitled main.c scanner.h scanner.c)