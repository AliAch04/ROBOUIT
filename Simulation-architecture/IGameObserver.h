#pragma once

#include <vector>
#include "Enums.h"
#include "Point.h"
#include "ScoreData.h"

class IGameObserver {
public:
    virtual ~IGameObserver() = default;
    virtual void onGameStateChanged(GameState newState) = 0;
    virtual void onRobotMoved(Point p) = 0;
    virtual void onPathFound(const std::vector<Point>& path) = 0;
    virtual void onLevelUp(int newLevel) = 0;
    virtual void onSolvingComplete(const ScoreData& score) = 0;
};
