cmake_minimum_required(VERSION 3.26)
project(zyerr C)

set(CMAKE_C_STANDARD 23)

include_directories(zyalloc/src)

add_library(zyerr src/zyerr.c
        zyalloc/src/zyalloc.c)
