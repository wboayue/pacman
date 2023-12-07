#include <fstream>
#include <iostream>

#include "grid.h"

bool Grid::HasPellet(const Vec2 &position) {
  return GetCell(position) == kPellet || GetCell(position) == kPowerPellet;
}

void Grid::Reset(SDL_Renderer *renderer) {
  cells = Grid::Load("../assets/maze.txt");
  CreatePellets(renderer);
}

std::unique_ptr<Pellet> Grid::ConsumePellet(const Vec2 &position) {
  auto it = pellets.find(position);
  if (it != pellets.end()) {
    auto tmp = std::move(it->second);
    pellets.erase(it);

    cells.at(position.y).at(position.x) = kBlank;

    return tmp;
  }

  return {};
}

void Grid::CreatePellets(SDL_Renderer *renderer) {
  for (int y = 0; y < Height(); ++y) {
    for (int x = 0; x < Width(); ++x) {
      Vec2 position{(float)x, (float)y};
      if (GetCell(position) == Cell::kPowerPellet) {
        pellets[position] = std::make_unique<Pellet>(renderer, position, true);
      } else if (GetCell(position) == Cell::kPellet) {
        pellets[position] = std::make_unique<Pellet>(renderer, position);
      }
    }
  }
}

void Grid::Update(const float deltaTime) {
  for (auto &pellet : pellets) {
    pellet.second->Update(deltaTime);
  }
}

void Grid::Render(SDL_Renderer *renderer) {
  for (auto &pellet : pellets) {
    pellet.second->Render(renderer);
  }
}

std::vector<std::vector<Cell>> Grid::Load(const std::string &gridPath) {
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
      case '-':
        row.push_back(Cell::kWall);
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
      std::cerr << "row " << y << " should have " << kGridWidth
                << " columns. found " << row.size() << "." << std::endl;
      std::cerr << line << std::endl;
      std::abort();
    }

    cells.push_back(row);
    y++;
  }

  if (cells.size() != kGridHeight) {
    std::cerr << "expected grid of " << kGridHeight << " rows. got "
              << cells.size() << std::endl;
    std::abort();
  }

  file.close();
  return std::move(cells);
}
