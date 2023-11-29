#include <iostream>
#include <fstream>

#include "grid.h"

Grid Grid::Load(const std::string &gridPath)
{
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
            switch (ch)
            {
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
            std::cerr << "row " << y << " should have " << kGridWidth << " columns. found " << row.size() << "." << std::endl;
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
    return std::move(Grid(cells));
}
