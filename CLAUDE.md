# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

### Prerequisites Installation (macOS)
```bash
brew install SDL2 SDL2_image SDL2_mixer
brew install llvm  # For clang-tidy
```

### Build and Run
```bash
# Create build directory and compile
mkdir -p build && cd build
cmake .. && make

# Run the game
./pacman
```

### Development Commands
```bash
# Clean rebuild
rm -rf build && mkdir build && cd build && cmake .. && make

# Build with specific compiler (optional)
cmake -DCMAKE_CXX_COMPILER=clang++ ..

# Run clang-tidy for linting
clang-tidy src/*.cpp -- -std=c++20 -I/usr/local/include/SDL2
```

## Architecture Overview

This is a C++20 Pacman game implementation using SDL2 for graphics and audio. The codebase follows a hybrid Entity-Component approach with object-oriented design patterns.

### Core Systems

**Game Loop** (`Game` class)
- Main game orchestrator managing the game loop (input → update → render cycle)
- Runs at 60 FPS (configurable in `main.cpp`)
- Manages all game entities and subsystems
- State management through friend struct pattern (ReadyState, PlayState, PausedState, DyingState, LevelCompleteState)

**Audio System** (`AudioSystem` class)
- Asynchronous sound playback using dedicated thread
- Queue-based request processing with thread-safe operations
- Supports intro, munch, power pellet, and death sounds

**Rendering System** (`Renderer` class)
- Handles all SDL2 rendering operations
- Manages textures and sprite drawing
- Works with `Sprite` class for entity visuals

**Asset Management** (`AssetManager` class)
- Centralized texture and asset loading
- Manages SDL_Texture resources

### Game Entities

**Core Entities**
- `Pacman`: Player character with movement and collision logic
- `Ghost`: AI-controlled enemies (Blinky, Pinky, Inky, Clyde) with different behaviors
- `Pellet`: Collectible items (regular and power pellets)
- `Grid`: Game board representation with maze layout

**Support Classes**
- `BoardManager`: Manages board state and pellet placement
- `GameContext`: Tracks game state (score, lives, level, paused state)
- `Vector2`: 2D vector math utilities for positions and movement

### Key Design Patterns

1. **State Pattern**: Game states managed through friend structs for clean state transitions
2. **Manager Pattern**: Dedicated managers for board, assets, and audio
3. **Entity Hybrid**: Entities are mostly data-driven with behavior logic in dedicated systems
4. **Thread Safety**: Audio system uses mutex and condition variables for thread-safe queue operations

### Ghost Modes
Ghosts operate in different modes defined in `GameContext`:
- `kScatter`: Ghosts move to corners
- `kChase`: Ghosts pursue Pacman
- `kScared`: Ghosts flee when power pellet is active
- `kReSpawn`: Ghost returning to spawn after being eaten

### Current Branch Structure
The repository uses feature branches for development. Main development happens on feature branches that are merged to `main` via pull requests.