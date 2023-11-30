#ifndef GRID_H
#define GRID_H

#include <fstream>
#include <vector>
#include <string>
#include <math.h>

#include "SDL.h"

#include "vector2.h"
#include "pellet.h"

enum Cell {
    kBlank,
    kWall,
    kPellet,
    kPowerPellet,
    kOffGrid
};

const int kGridWidth = 28;
const int kGridHeight = 36;

class Grid {
public:
    Grid(){};
    Grid(std::vector<std::vector<Cell>> cells)
     : cells{cells}
    {

    };
    
    void Update(const float deltaTime);
    void Render(SDL_Renderer *renderer);

    int Width() const {
        return cells.at(0).size();
    };
    
    int Height() const {
        return cells.size();
    };

    const Cell GetCell(const Vec2 &position) const {
        if (position.x < 0 || position.y < 0) {
            return Cell::kOffGrid;
        }
        if (floor(position.x) >= kGridWidth) {
            return Cell::kOffGrid;
        }
        return cells.at(floor(position.y)).at(floor(position.x));
    };

    bool HasPellet(const Vec2 &position);
    std::unique_ptr<Pellet> ConsumePellet(const Vec2 &position);

    void CreatePellets(SDL_Renderer *renderer);

//    Grid& operator=(const Grid& grid);
    static Grid Load(const std::string &gridPath);
private:
    std::vector<std::vector<Cell>> cells;
    // std::vector<std::unique_ptr<Pellet>> pellets;
    std::unordered_map<Vec2, std::unique_ptr<Pellet>, Vec2Hash> pellets;
};

#endif