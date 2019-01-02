cmake_minimum_required(VERSION 3.13)
project(LZS_SHF C)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES sources/Main.c sources/LZSFunctions.c sources/ShannonFanoFunctions.c headers/ShannonFanoFunctions.h headers/LZSFunctions.h)

add_executable(LZS_SHF ${SOURCE_FILES})