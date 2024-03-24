cmake_minimum_required(VERSION 3.7)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-std=c++20)

add_compile_options(-Wall -Wextra -Wpedantic)

project(pacman)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED >=2.0.0)
find_package(SDL2_mixer REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} ${SDL2_MIXER_INCLUDE_DIRS} src)

# setup clang-tidy
find_program(CLANG_TIDY_EXE NAMES "clang-tidy" REQUIRED)
#set(CLANG_TIDY_COMMAND "${CLANG_TIDY_EXE}" "-checks=-*,modernize-*,readability-*,cppcoreguidelines-*,bugprone-*")
set(CLANG_TIDY_COMMAND "${CLANG_TIDY_EXE}" "-checks=-*,modernize-*,cppcoreguidelines-*")

add_executable(pacman 
    src/main.cpp
    src/audio-system.cpp
    src/game.cpp
    src/renderer.cpp
    src/sprite.cpp
    src/pacman.cpp
    src/vector2.cpp
    src/grid.cpp
    src/pellet.cpp
    src/game-state.cpp
    src/ghost.cpp
    src/board-manager.cpp)

set_target_properties(pacman PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")

string(STRIP ${SDL2_LIBRARIES} SDL2_LIBRARIES)
target_link_libraries(pacman ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${SDL2_MIXER_LIBRARIES})
