cmake_minimum_required(VERSION 3.8.2)
project(Interpreter C)

set(CMAKE_C_STANDARD 99)

add_executable(Interpreter main.c headers/instructions.h commands.c headers/main.h headers/commands.h util.c headers/util.h
        testing/testing.c log.c headers/log.h csv.c gui.c headers/gui.h)