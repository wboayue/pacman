#include <algorithm>
#include <fstream>
#include <iostream>

#include "grid.h"

auto Grid::HasPellet(const Vec2 &position) const -> bool {
  return GetCell(position) == Cell::kPellet || GetCell(position) == Cell::kPowerPellet;
}

auto Grid::Reset(SDL_Renderer *renderer) -> void {
  cells = Grid::Load("../assets/maze.txt");
  CreatePellets(renderer);
}

auto Grid::ConsumePellet(const Vec2 &position) -> std::unique_ptr<Pellet> {
  auto it = pellets.find(position);
  if (it != pellets.end()) {
    auto tmp = std::move(it->second);
    pellets.erase(it);

    cells.at(position.y).at(position.x) = Cell::kBlank;

    return tmp;
  }

  return {};
}

auto Grid::CreatePellets(SDL_Renderer *renderer) -> void {
  for (int y = 0; y < Height(); ++y) {
    for (int x = 0; x < Width(); ++x) {
      Vec2 position{static_cast<float>(x), (float)y};
      if (GetCell(position) == Cell::kPowerPellet) {
        pellets[position] = std::make_unique<Pellet>(renderer, position, true);
      } else if (GetCell(position) == Cell::kPellet) {
        pellets[position] = std::make_unique<Pellet>(renderer, position);
      }
    }
  }
}

auto Grid::Update(const float deltaTime) -> void {
  for (auto &pellet : pellets) {
    pellet.second->Update(deltaTime);
  }
}

auto Grid::Render(SDL_Renderer *renderer) -> void {
  for (auto &pellet : pellets) {
    pellet.second->Render(renderer);
  }
}

auto Grid::Load(const std::string &gridPath) -> std::vector<std::vector<Cell>> {
  std::fstream file{gridPath};
  if (!file.is_open()) {
    std::cerr << "Unable to open grid at: " << gridPath << std::endl;
    std::abort();
  }

  std::vector<std::vector<Cell>> cells;

  int y = 1;
  std::string line;
  while (std::getline(file, line)) {
    std::vector<Cell> row;
    row.reserve(line.size());

    for (auto &ch : line) {
      switch (ch) {
      case '#':
        row.push_back(Cell::kWall);
        break;

      case '-':
        row.push_back(Cell::kGate);
        break;

      case '.':
        row.push_back(Cell::kPellet);
        break;

      case '*':
        row.push_back(Cell::kPowerPellet);
        break;

      default:
        row.push_back(Cell::kBlank);
        break;
      }
    }

    if (row.size() != kGridWidth) {
      std::cerr << "row " << y << " should have " << kGridWidth << " columns. found " << row.size()
                << "." << std::endl;
      std::cerr << line << std::endl;
      std::abort();
    }

    cells.push_back(row);
    y++;
  }

  if (cells.size() != kGridHeight) {
    std::cerr << "expected grid of " << kGridHeight << " rows. got " << cells.size() << std::endl;
    std::abort();
  }

  file.close();
  return cells;
}
