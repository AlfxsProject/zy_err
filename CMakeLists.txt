cmake_minimum_required(VERSION 3.26)
project(zyerr C)

set(CMAKE_C_STANDARD 23)

add_library(zyerr zyerr.c
        zyalloc/library.c)
