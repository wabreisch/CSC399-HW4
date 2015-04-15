cmake_minimum_required(VERSION 3.1)
project(Program4)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    builtins.c
    builtins.h
    command.c
    command.h
    global.h
    quShell.c
    tokenizer.c
    tokenizer.h
    varSet.c
    varSet.h)

add_executable(Program4 ${SOURCE_FILES})