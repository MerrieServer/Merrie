cmake_minimum_required(VERSION 3.14)

# Project
project(Merrie)

# Options
option(MERRIE_USE_OPENSSL                    "Should OpenSSL be used?"                               ON)
option(MERRIE_DO_UNIT_TESTS                  "Should unit tests be compiled and run?"                ON)
option(MERRIE_COMPILE_GAME_SERVER            "Should the gameserver be compiled?"                    ON)
option(MERRIE_COMPILE_GAME_TOOLS             "Should the game tools be compiled?"                    ON)

# Global properties
set(CMAKE_CXX_STANDARD 20)

# Compiler flags
# gcc
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # Warnings
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")

    # PIC & PIE
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIE -fPIC")

    # Optimizations
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
endif ()

# MSVC
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # Runtime
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")

    # Warnings
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")

    # Optimizations
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /Oy")
endif ()

# Subprojects
add_subdirectory("Commons")

if (MERRIE_COMPILE_GAME_SERVER)
    add_subdirectory("GameServer")
endif ()

if (MERRIE_COMPILE_GAME_TOOLS)
    add_subdirectory("GameTools")
endif ()
