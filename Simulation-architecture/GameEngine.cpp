#include "GameEngine.h"
#include "ScoreCalculator.h"
#include <iostream>

GameEngine::GameEngine() {
    playerRobot = std::make_unique<Robot>();
    pathFinder = std::make_unique<PathFinder>();
    fileManager = std::make_unique<FileManager>();
    gameState = GameState::IDLE;
    pathIndex = 0;
    solvingTime = 0.0f;
}

void GameEngine::addObserver(IGameObserver* observer) {
    observers.push_back(observer);
}

void GameEngine::loadLevel() {
    std::vector<std::string> levelMap = {
        "##########",
        "#S...#...#",
        "###.####.#",
        "#...#....#",
        "#.####.#.#",
        "#......#E#",
        "##########"
    };
    currentMaze = std::make_unique<Maze>();
    currentMaze->loadFromMap(levelMap);
    playerRobot->setPosition(currentMaze->startPos);

    for (auto* obs : observers) obs->onGameStateChanged(GameState::IDLE);
}

void GameEngine::startSolving() {
    gameState = GameState::SOLVING;
    playerRobot->setState(RobotState::CALCULATING);
    for (auto* obs : observers) obs->onGameStateChanged(GameState::SOLVING);

    currentPath = pathFinder->findPath(currentMaze.get());
    for (auto* obs : observers) obs->onPathFound(currentPath);

    pathIndex = 0;
    if (currentPath.empty()) gameState = GameState::FAILED;
}

void GameEngine::update(float deltaTime) {
    if (gameState != GameState::SOLVING) return;
    solvingTime += deltaTime;

    // Simulate movement logic (simplified timer)
    static float moveTimer = 0.0f;
    moveTimer += deltaTime;

    if (moveTimer > 0.3f && pathIndex < currentPath.size()) {
        Point nextPos = currentPath[pathIndex++];
        playerRobot->setPosition(nextPos);
        playerRobot->setState(RobotState::MOVING);

        for (auto* obs : observers) obs->onRobotMoved(nextPos);
        moveTimer = 0.0f;

        if (nextPos == currentMaze->endPos) {
            gameState = GameState::COMPLETE;
            playerRobot->setState(RobotState::COMPLETED);

            ScoreData score = ScoreCalculator::calculate(currentPath, solvingTime, DifficultyLevel::MEDIUM);
            playerRobot->addExperience(score.experienceGained);

            // Phase 5: Persistence
            fileManager->saveRobot(*playerRobot, "save_game.json");

            for (auto* obs : observers) {
                obs->onSolvingComplete(score);
                obs->onGameStateChanged(GameState::COMPLETE);
            }
        }
    }
}



