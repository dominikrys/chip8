cmake_minimum_required(VERSION 3.10)

project(chip-8-emulator)

set(CMAKE_CXX_STANDARD 17)

set(SRCS
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
        src/Constants.h
        src/Timer.h
        src/Mode.h
        src/Config.h)

if (WIN32 OR UNIX AND NOT EMSCRIPTEN)
    set(SRCS ${SRCS} src/Main.cpp)
else ()
    set(SRCS ${SRCS} src/MainEmscripten.cpp)
endif ()

add_executable(${PROJECT_NAME} ${SRCS})

set(CMAKE_CXX_FLAGS "\
    -std=c++17 \
    -Werror \
    -Wall \
    -Wextra \
    -Wshadow \
    -pedantic")

set(CMAKE_EXE_LINKER_FLAGS "\
    -static-libgcc \
    -static-libstdc++")

if (WIN32)
    if (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
        set(ARCH "x86_64-w64-mingw32") # 64-bit
    else ()
        set(ARCH "i686-w64-mingw32") # 32-bit
    endif ()

    add_definitions(-DSDL_MAIN_HANDLED) # Allow console output

    set(SDL2_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/external/SDL2-2.0.10/${ARCH}/include/")
    set(SDL2_LIBPATH "${CMAKE_CURRENT_LIST_DIR}/external/SDL2-2.0.10/${ARCH}/lib/")

    find_library(SDL2_LIB SDL2 PATH ${SDL2_LIBPATH})
    find_library(SDL2_MAIN_LIB SDL2main PATH ${SDL2_LIBPATH})
    set(SDL2_LIBRARIES ${SDL2_LIB} ${SDL2_MAIN_LIB})

elseif (UNIX AND NOT EMSCRIPTEN)
    include(FindPkgConfig)
    pkg_search_module(SDL2 REQUIRED sdl2 >= 2.0.0)

    set(CMAKE_C_COMPILER "gcc-9")
    set(CMAKE_CXX_COMPILER "g++-9")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

elseif (EMSCRIPTEN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
        -O2 \
        -s USE_SDL=2 \
        -s WASM=1 \
        -s EXPORTED_FUNCTIONS=\"['_main', '_loadRom']\" \
        -s EXPORTED_RUNTIME_METHODS=\"['ccall', 'cwrap']\" \
        -s ALLOW_MEMORY_GROWTH=1 \
        --no-heap-copy \
        --preload-file ../bin \
        --shell-file ../web/shell_minimal.html")

    target_link_libraries(${PROJECT_NAME} "-o ${PROJECT_NAME}.html")

endif ()

include_directories(${SDL2_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
