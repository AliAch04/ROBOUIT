#include "Maze.h"

void Maze::loadFromMap(const std::vector<std::string>& layout) {
    height = layout.size();
    if (height == 0) return;
    width = layout[0].size();

    grid.clear();
    grid.resize(height);

    for (int y = 0; y < height; ++y) {
        grid[y].reserve(width);
        for (int x = 0; x < width; ++x) {
            char c = layout[y][x];
            CellType t = CellType::EMPTY;
            if (c == '#') t = CellType::WALL;
            else if (c == 'S') t = CellType::START;
            else if (c == 'E') t = CellType::END;

            grid[y].push_back(Cell::create(t, { x, y }));
            if (t == CellType::START) startPos = { x, y };
            if (t == CellType::END) endPos = { x, y };
        }
    }
}

bool Maze::isValid(Point p) const {
    return p.x >= 0 && p.x < width && p.y >= 0 && p.y < height;
}

bool Maze::isWall(Point p) const {
    if (!isValid(p)) return true;
    return grid[p.y][p.x]->getType() == CellType::WALL;
}


