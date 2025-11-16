#ifndef GRID_H
#define GRID_H

#include <fstream>
#include <math.h>
#include <string>
#include <vector>

#include "SDL.h"

#include "pellet.h"
#include "vector2.h"

enum class Cell { kBlank, kWall, kGate, kPellet, kPowerPellet, kOffGrid };

const int kGridWidth = 28;
const int kGridHeight = 36;

class Grid {
public:
  Grid() {};
  Grid(std::vector<std::vector<Cell>> cells) : cells{cells} {};

  auto Update(const float deltaTime) -> void;
  auto Render(SDL_Renderer *renderer) -> void;

  auto Width() const -> int { return cells.at(0).size(); };

  auto Height() const -> int { return cells.size(); };

  auto GetCell(const Vec2 &position) const -> Cell {
    if (position.x < 0 || position.y < 0) {
      return Cell::kOffGrid;
    }
    if (floor(position.x) >= kGridWidth) {
      return Cell::kOffGrid;
    }
    return cells.at(floor(position.y)).at(floor(position.x));
  };

  auto HasPellet(const Vec2 &position) const -> bool;
  auto ConsumePellet(const Vec2 &position) -> std::unique_ptr<Pellet>;

  auto Reset(SDL_Renderer *renderer) -> void;

  auto CreatePellets(SDL_Renderer *renderer) -> void;

  auto static Load(const std::string &gridPath) -> std::vector<std::vector<Cell>>;

private:
  std::vector<std::vector<Cell>> cells;
  std::unordered_map<Vec2, std::unique_ptr<Pellet>, Vec2Hash> pellets;
};

#endif