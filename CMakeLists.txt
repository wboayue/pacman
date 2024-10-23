cmake_minimum_required(VERSION 3.7)
project(pacman
    VERSION 1.0.0
    DESCRIPTION "Pacman game implementation"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSION OFF)

# Define source files
set(SOURCES
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
    src/board-manager.cpp
)

# Add custom cmake modules path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Find required packages
find_package(SDL2 REQUIRED)
find_package(SDL2_image 2.0.0 REQUIRED)
find_package(SDL2_mixer REQUIRED)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Set compile options
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_IMAGE_INCLUDE_DIRS}
    ${SDL2_MIXER_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Configure clang-tidy
find_program(CLANG_TIDY_EXE NAMES "clang-tidy" REQUIRED)
#set(CLANG_TIDY_COMMAND "${CLANG_TIDY_EXE}" "-checks=-*,modernize-*,readability-*,cppcoreguidelines-*,bugprone-*")
set(CLANG_TIDY_COMMAND "${CLANG_TIDY_EXE}" "-checks=-*,modernize-*")

# Link libraries
string(STRIP ${SDL2_LIBRARIES} SDL2_LIBRARIES)
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${SDL2_LIBRARIES}
    ${SDL2_IMAGE_LIBRARIES}
    ${SDL2_MIXER_LIBRARIES}
)

# Install rules
include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
