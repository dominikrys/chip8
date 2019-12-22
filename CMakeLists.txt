cmake_minimum_required(VERSION 3.15)
project(chip_8)

set(CMAKE_CXX_STANDARD 17)

add_executable(chip_8 main.cpp chip8.h chip8.cpp)