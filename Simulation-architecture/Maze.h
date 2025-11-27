#pragma once
#include <vector>
#include <string>
#include <memory>
#include "Cell.h"

class Maze {
public:
    int width = 0;
    int height = 0;
    Point startPos{ 0,0 };
    Point endPos{ 0,0 };
    std::vector<std::vector<std::unique_ptr<Cell>>> grid;

    Maze() = default;
    void loadFromMap(const std::vector<std::string>& layout);
    bool isValid(Point p) const;
    bool isWall(Point p) const;
};

