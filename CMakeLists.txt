cmake_minimum_required(VERSION 3.15)
project(chip_8)

set(CMAKE_CXX_STANDARD 17)

add_executable(chip_8 src/main.cpp src/chip8.h src/chip8.cpp)

set_property(TARGET chip_8 PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/fuck")
