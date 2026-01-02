# Pacman

A modern C++20 implementation of the classic Pac‑Man, built with SDL2 and CMake. The codebase uses a clean state machine, resource managers, and an entity-centric design for clarity and extensibility.

![Gameplay Screenshot](assets/screenshots/gameplay.png)

## Architecture Overview
- States: Ready, Play, Paused, Dying, LevelComplete (centralized transitions).
- Managers: `AssetManager` (textures), `AudioSystem` (async sound queue), `BoardManager` (maze & rules).
- Entities: `Pacman`, `Ghost`(s), `Pellet` with shared `GameContext` for coordination.
- Rendering & Input: `Renderer` handles drawing; `Game` orchestrates loop and input.

## Project Layout
- `src/` core modules (e.g., `game.*`, `renderer.*`, `board-manager.*`, `asset-manager.*`, `audio-system.*`, `pacman.*`, `ghost.*`, `pellet.*`, `grid.*`, `vector2.*`).
- `assets/` sprites, sounds, and level data (e.g., `maze.txt`).
- `CMakeLists.txt` build configuration; out-of-tree builds recommended (`build/`).

## Prerequisites
- CMake ≥ 3.7, Make, and a C++20 compiler.
- SDL2, SDL2_image, SDL2_mixer.
- clang-tidy (optional; auto-enabled if found) and clang-format (optional targets).

macOS (Homebrew):
`brew install sdl2 sdl2_image sdl2_ttf sdl2_mixer llvm`

Ubuntu/Debian:
`sudo apt-get install cmake build-essential libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev`

## Build & Run
```bash
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j
./pacman
```
Notes
- Run from the `build/` directory so asset relative paths resolve.
- clang-tidy is optional. If installed, it runs automatically; disable via `-DENABLE_CLANG_TIDY=OFF`.

## Development
- Formatting: `cmake --build build --target clang-format` or run `clang-format -i src/*.cpp src/*.h` (uses `.clang-format`).
- Linting: clang-tidy is configured (modernize checks). You can expand checks via CMake if desired.
- Recommendations: avoid magic numbers (see `constants.h`), prefer RAII and `std::string_view`, cache textures via `AssetManager`.

## Testing (Optional)
- Suggested: GoogleTest/Catch2 in `tests/` integrated with CTest; name files `<module>_test.cpp` and run `ctest` from `build/`.

## Credits
Assets are included in `assets/`. Audio references and attributions can be added per asset source.
