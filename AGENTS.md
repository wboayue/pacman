# Repository Guidelines

## Project Structure & Module Organization
- Source: `src/` (core modules: `game.*`, `renderer.*`, `board-manager.*`, `asset-manager.*`, `audio-system.*`, entities like `pacman.*`, `ghost.*`, `pellet.*`).
- Assets: `assets/` (images, sounds, level data like `maze.txt`).
- Build config: `CMakeLists.txt`. Create an out-of-tree `build/` directory.
- Reference: see `CODE_REVIEW.md` for architectural notes (state machine, managers).

## Build, Test, and Development Commands
- Configure & build (Debug):
  - `mkdir -p build && cd build`
  - `cmake .. -DCMAKE_BUILD_TYPE=Debug && make -j` (requires SDL2 and extensions)
- Run locally: `./pacman` from `build/`.
- Install deps (macOS examples):
  - `brew install sdl2 sdl2_image sdl2_ttf sdl2_mixer llvm`
- Format code: `clang-format -i src/*.cpp src/*.h` (uses `.clang-format`).
- Optional tests: integrate with CTest; run via `ctest --output-on-failure` in `build/` once tests exist.

## Coding Style & Naming Conventions
- Language: C++20. Indent 2 spaces, line length 100 (enforced by `.clang-format`).
- Naming: constants use `kPascalCase` (e.g., `kTileSize`); member variables end with `_`; prefer `snake_or-kebab` file names to match existing.
- Practices: RAII, const-correctness, avoid magic numbers (extract to `constants.h`), prefer smart pointers and `std::string_view` where applicable.

## Testing Guidelines
- Framework: GoogleTest or Catch2 in `tests/`, added via CMake; name files `<module>_test.cpp`.
- Scope: unit tests for managers (Asset/Audio/Board) and state transitions; fast, deterministic.
- Run: `ctest` from `build/`. Aim for coverage on core logic; exclude SDL initialization paths.

## Commit & Pull Request Guidelines
- Commits observed: imperative mood with occasional prefixes (`chore:`, `refactor:`) and merge commits. Follow: short subject (<72 chars), descriptive body, reference issues.
- PRs: include summary, rationale, before/after notes, and run instructions. Attach screenshots/gifs for gameplay changes. Link related issues.

## Security & Configuration Tips
- Avoid hardcoded asset paths; resolve relative to executable or configurable base path.
- Validate window and input dimensions to prevent crashes (see notes in `CODE_REVIEW.md`).
- Handle SDL errors robustly and log failures; fail gracefully when assets are missing.
