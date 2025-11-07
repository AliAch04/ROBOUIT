// Maze.hpp
#pragma once
#include <vector>
#include <string>

class Maze {
public:
    bool load(const std::vector<std::string>& lines);

    bool isWall(int x, int y) const;
    int width() const { return w; }
    int height() const { return h; }

    std::pair<int, int> start;
    std::pair<int, int> goal;

private:
    int w = 0, h = 0;
    std::vector<int> grid;  // 0 = free, 1 = wall
};