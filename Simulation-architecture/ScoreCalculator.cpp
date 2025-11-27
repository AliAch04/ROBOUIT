#include "ScoreCalculator.h"
#include <cmath>

ScoreData ScoreCalculator::calculate(const std::vector<Point>& path, float timeTaken, DifficultyLevel diff) {
    ScoreData data;
    data.baseScore = 1000;
    data.timeBonus = std::max(0, 500 - (int)timeTaken * 10);

    // Simple difficulty multiplier
    switch (diff) {
    case DifficultyLevel::EASY: data.difficultyMultiplier = 1.0f; break;
    case DifficultyLevel::MEDIUM: data.difficultyMultiplier = 1.5f; break;
    case DifficultyLevel::HARD: data.difficultyMultiplier = 2.0f; break;
    default: data.difficultyMultiplier = 1.0f;
    }

    data.totalScore = (data.baseScore + data.timeBonus) * data.difficultyMultiplier;
    data.experienceGained = data.totalScore / 10;
    return data;
}