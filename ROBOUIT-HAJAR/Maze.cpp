// Maze.cpp
#include "Maze.hpp"

bool Maze::load(const std::vector<std::string>& lines) {
    if (lines.empty()) return false;

    h = lines.size();
    w = lines[0].size();
    grid.assign(w * h, 1);

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            char c = lines[y][x];
            if (c == '#') grid[y * w + x] = 1;
            else grid[y * w + x] = 0;

            if (c == 'S') start = { x,y };
            if (c == 'G') goal = { x,y };
        }
    }
    return true;
}

bool Maze::isWall(int x, int y) const {
    return grid[y * w + x] == 1;
}