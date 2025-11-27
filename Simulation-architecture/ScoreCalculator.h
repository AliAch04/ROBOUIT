#pragma once

#include "ScoreData.h"
#include "Point.h"
#include "Enums.h"
#include <vector>

class ScoreCalculator {
public:
    static ScoreData calculate(const std::vector<Point>& path, float timeTaken, DifficultyLevel diff);
};



