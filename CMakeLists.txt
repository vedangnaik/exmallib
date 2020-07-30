cmake_minimum_required(VERSION 3.10)
project(exmallibProject VERSION 1.0)

add_library(
    exmallib STATIC
    exmallib.h
    exmallib.c
    helpers.c
)

add_executable(exmallibProject main.c)
target_link_libraries(exmallibProject PRIVATE exmallib)