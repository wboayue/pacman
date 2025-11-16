# Pacman C++20 Project Review

**Date:** 2025-11-16
**Reviewer:** Senior C++ Game Developer
**Codebase:** 2,530 lines across 27 files

---

## What's Being Done Well

### Architecture & Design
- **Clean state machine pattern** - Game states (Ready/Play/Paused/Dying/LevelComplete) are well-separated with clear transitions
- **Extensible ghost AI system** - Function pointer-based targeters make adding new ghost behaviors trivial
- **Thread-safe audio** - Dedicated thread with mutex/condition variable prevents main loop blocking
- **RAII compliance** - Smart pointers (`std::unique_ptr`, `std::shared_ptr`) throughout ensure proper resource cleanup

### Modern C++ Usage
- Consistent use of C++20 features: `auto` returns, trailing return types, `constexpr`, `std::optional`, lambdas
- Proper const correctness (~90% compliance)
- Strong naming conventions: `kConstants`, `memberVariables_`, PascalCase types

### Code Quality
- Clear separation of concerns (Game/Renderer/Audio/Entities)
- Cell-based collision system is elegant and performant
- Ghost personalities (Blinky/Pinky/Inky/Clyde) faithfully implemented
- Good use of `std::unordered_map` with custom hash for pellet storage

---

## Critical Issues

### 1. Silent Initialization Failures
```cpp
// game.cpp:31-35
if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
  std::cerr << "SDL could not initialize.\n";
  return;  // Constructor succeeds in broken state!
}
```
**Fix:** Throw exceptions for critical failures

### 2. Division by Zero
```cpp
// vector2.cpp:31-36
if (rhs != 0) { return {x / rhs, y / rhs}; }
else { return {std::numeric_limits<float>::infinity(), 0}; }
```
**Fix:** Throw `std::domain_error` or assert

### 3. Unchecked Window Resize
```cpp
// game.cpp:131
float newAspectRatio = (float)newWidth / (float)newHeight; // Zero check missing
```

### 4. Aggressive `std::abort()` on Missing Assets
`sprite.cpp:69` crashes the entire program if a texture fails to load

---

## High Priority Improvements

### 1. Complete AssetManager (Currently 14 lines)
```cpp
// Current - loads from disk every call
auto Game::GetTexture(const std::string &file) {
  return LoadTexture(renderer_->sdl_renderer, file);
}
```

**Recommendation:** Add texture caching:
```cpp
class AssetManager {
  std::unordered_map<std::string, SDL_Texture*> textureCache_;
public:
  auto GetTexture(const std::string& path) -> SDL_Texture* {
    if (auto it = textureCache_.find(path); it != textureCache_.end()) {
      return it->second;
    }
    auto* tex = LoadTexture(path);
    textureCache_[path] = tex;
    return tex;
  }
};
```

### 2. Implement Rule of 5 for Sprite
`sprite.h:16` has a `// TODO` comment - missing copy/move constructors and assignment operators

### 3. No Unit Tests
- No test framework integrated
- Easily testable components: `Vec2` math, `Grid` validation, state transitions
- Add GoogleTest or Catch2 to CMakeLists.txt

### 4. Hardcoded Relative Asset Paths
```cpp
// audio-system.cpp
return "../assets/sounds/munch_1.wav";  // Fragile - must run from build/
```
**Fix:** Use environment variable or config file for base path

---

## Medium Priority

### Code Duplication
Pause/resume logic repeated in `PlayState::Tick()` (lines 194-205) and `PausedState::Tick()` (lines 290-302)

### Magic Numbers
```cpp
static constexpr float kEnergizerDuration = 6.0f;
static constexpr int kReadyStateDuration = 4;
static constexpr int kGhostScore = 50;
```
Many constants are scattered or inline - consolidate into `constants.h`

### Public SDL Renderer
`renderer.h:21` - `SDL_Renderer *sdl_renderer` is public, breaking encapsulation. Add getters or friend access.

### Vec2 Performance
```cpp
// Current - pow() is slower
sqrt(pow(diff.x, 2.0) + pow(diff.y, 2.0))

// Better
sqrt(diff.x * diff.x + diff.y * diff.y)
```

---

## Low Priority Enhancements

1. **String allocations in hot path** - `getSoundFile()` returns `std::string` by value; use `std::string_view`
2. **Add debug overlay** for FPS, ghost modes, pathfinding visualization
3. **Doxygen coverage** - AudioSystem is well-documented, but Game/Pacman/Ghost need more
4. **Consider `std::variant`** for state machine instead of map of base class pointers

---

## Overall Assessment

**Grade: 7.5/10**

| Category | Score | Notes |
|----------|-------|-------|
| Architecture | 8/10 | Clean separation, extensible design |
| Modern C++ | 8.5/10 | Good C++20 usage, consistent style |
| Error Handling | 4/10 | Silent failures, abort on errors |
| Testing | 0/10 | No tests present |
| Performance | 7/10 | Good for current scale, lacks caching |

This is a solid foundation with professional architectural decisions. The main gaps are robustness (error handling) and completeness (AssetManager, tests, documentation). The core gameplay logic is well-implemented and extensible.
