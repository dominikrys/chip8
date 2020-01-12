cmake_minimum_required(VERSION 3.15)
project(chip_8)

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME}
        src/Main.cpp
        src/Chip8.h
        src/Chip8.cpp
        src/Renderer.cpp
        src/Renderer.h
        src/KeyboardHandler.cpp
        src/KeyboardHandler.h
        src/Configurator.cpp
        src/Configurator.h
        src/Audio.cpp
        src/Audio.h
        src/Mode.h)

# Set linker and compiler options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Werror -Wall -Wextra -Wshadow -pedantic" #[[-Weffc++ -lstdc++fs]])
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

# SDL
set(SDL2_DIR C:/SDL2-2.0.10/x86_64-w64-mingw32)
set(SDL2_LIB_DIR ${SDL2_DIR}/lib)
include_directories(${SDL2_DIR}/include)
target_link_libraries(${PROJECT_NAME}
        ${SDL2_LIB_DIR}/libSDL2.dll.a
        ${SDL2_LIB_DIR}/libSDL2main.a
        #[[-mwindows]])
add_definitions(-DSDL_MAIN_HANDLED)

set(ENV{SDL_AUDIODRIVER} "winmm")