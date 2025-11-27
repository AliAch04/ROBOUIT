#pragma once
#include "IHeuristic.h"
#include <cmath>

class ManhattanHeuristic : public IHeuristic {
public:
    float calculate(Point current, Point goal) override {
        return static_cast<float>(std::abs(current.x - goal.x) + std::abs(current.y - goal.y));
    }
};



