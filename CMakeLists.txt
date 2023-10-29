cmake_minimum_required(VERSION 3.26)
project(zy_err C)

set(CMAKE_C_STANDARD 23)

include_directories(zy_alloc zy_dequeue)

file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true "zy_alloc/*.h" "zy_alloc/*.c" "zy_dequeue/*.h" "zy_dequeue/*.c")

set(SOURCES ${SOURCES} zy_err.h zy_err.c)

add_library(${CMAKE_PROJECT_NAME} STATIC ${SOURCES})