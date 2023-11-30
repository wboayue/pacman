cmake_minimum_required(VERSION 3.7)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-std=c++17)

set(CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS, "${CXX_FLAGS}")

project(pacman)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED >=2.0.0)

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} src)

add_executable(pacman 
    src/main.cpp
    src/game.cpp
    src/renderer.cpp
    src/sprite.cpp
    src/pacman.cpp
    src/vector2.cpp
    src/grid.cpp
    src/pellet.cpp
    src/game-state.cpp
    src/board-manager.cpp)
string(STRIP ${SDL2_LIBRARIES} SDL2_LIBRARIES)
target_link_libraries(pacman ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})
