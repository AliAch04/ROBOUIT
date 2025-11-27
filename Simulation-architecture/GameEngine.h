#pragma once
#include <memory>
#include <vector>
#include "Robot.h"
#include "Maze.h"
#include "PathFinder.h"
#include "IGameObserver.h"
#include "FileManager.h"

class GameEngine {
private:
    std::unique_ptr<Maze> currentMaze;
    std::unique_ptr<Robot> playerRobot;
    std::unique_ptr<PathFinder> pathFinder;
    std::unique_ptr<FileManager> fileManager;

    std::vector<IGameObserver*> observers;
    std::vector<Point> currentPath;
    size_t pathIndex;
    GameState gameState;
    float solvingTime;

public:
    GameEngine();
    void loadLevel();
    void addObserver(IGameObserver* observer);
    void startSolving();
    void update(float deltaTime); // Frame update
    void notifyObservers(const std::string& eventType);

    const Maze& getMaze() const { return *currentMaze; }
    const Robot& getRobot() const { return *playerRobot; }
};




