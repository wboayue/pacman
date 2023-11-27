#ifndef GRID_H
#define GRID_H

#include <fstream>
#include <vector>
#include <string>
#include <math.h>

#include "vector2.h"

enum Cell {
    kBlank,
    kWall,
    kPellet,
    kPowerPellet
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
    
    int Width() const {
        return cells.at(0).size();
    };
    
    int Height() const {
        return cells.size();
    };

    Cell& GetCell(const Vec2 &position) {
        return cells.at(floor(position.y)).at(floor(position.x));
    };
//    Grid& operator=(const Grid& grid);
    static Grid Load(const std::string &gridPath);
private:
    std::vector<std::vector<Cell>> cells;
};

#endif