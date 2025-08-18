cmake_minimum_required(VERSION 3.21)

project(DragonC C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

add_executable(hatch hatch.c lex.c)
