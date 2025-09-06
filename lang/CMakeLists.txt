cmake_minimum_required(VERSION 3.21)

project(DragonC C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

add_executable(hatch)

target_include_directories(hatch PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_sources(hatch PRIVATE 
	hatch.c 
	lex.c 
	map.c 
	syntax.c 
	expr.c
	statement.c
	program.c
)
