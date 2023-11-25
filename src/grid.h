#ifndef GRID_H
#define GRID_H

#include <fstream>
#include <vector>
#include <string>

enum Cell {
    kBlank,
    kWall,
    kPellet,
    kPowerUp
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
    int Width() {
        return cells[0].size();
    };
    int Height() {
        return cells.size();
    };
//    Grid& operator=(const Grid& grid);
    static Grid Load(const std::string &gridPath);
private:
    std::vector<std::vector<Cell>> cells;
};

#endif