cmake_minimum_required(VERSION 3.16)
project(pacman
    VERSION 1.0.0
    DESCRIPTION "Pacman game implementation"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Define source files
set(SOURCES
    src/main.cpp
    src/asset-registry.cpp
    src/audio-system.cpp
    src/game.cpp
    src/renderer.cpp
    src/sprite.cpp
    src/pacman.cpp
    src/vector2.cpp
    src/grid.cpp
    src/pellet.cpp
    src/game-context.cpp
    src/ghost.cpp
    src/board-manager.cpp
    src/asset-manager.cpp
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

target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Configure clang-tidy (optional)
option(ENABLE_CLANG_TIDY "Enable clang-tidy static analysis" ON)
if(ENABLE_CLANG_TIDY)
  find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
  if(CLANG_TIDY_EXE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_CLANG_TIDY "${CLANG_TIDY_EXE};-checks=-*,modernize-*"
    )
    message(STATUS "clang-tidy enabled: ${CLANG_TIDY_EXE}")
  else()
    message(STATUS "clang-tidy not found. Static analysis disabled.")
  endif()
endif()

# Link libraries (using imported targets)
target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL2::SDL2
    SDL2::Image
    SDL2::Mixer
)

# Install rules
include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Clang-Format convenience targets
find_program(CLANG_FORMAT_EXE NAMES clang-format)
if(CLANG_FORMAT_EXE)
  file(GLOB_RECURSE ALL_CXX_SOURCE_FILES
       "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
       "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
       "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
       "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx")

  add_custom_target(clang-format
    COMMAND "${CLANG_FORMAT_EXE}" -i --style=file ${ALL_CXX_SOURCE_FILES}
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMENT "Formatting sources with clang-format")

  add_custom_target(clang-format-check
    COMMAND "${CLANG_FORMAT_EXE}" --dry-run --Werror --style=file ${ALL_CXX_SOURCE_FILES}
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMENT "Checking source formatting with clang-format")
else()
  message(STATUS "clang-format not found. 'clang-format' targets are unavailable.")
endif()
