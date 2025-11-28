#pragma once
#include <SFML/Graphics.hpp>
#include "GameEngine.h"
#include "MazeRenderer.h"
#include "IGameObserver.h"

class GameScreen : public IGameObserver {
private:
    sf::RenderWindow window;
    GameEngine engine;
    MazeRenderer mazeRenderer;
    sf::Clock deltaClock;

    // UI Elements (Basic)
    sf::CircleShape robotShape;
    std::vector<sf::RectangleShape> pathShapes; // For path visualization

public:
    GameScreen();
    void run();

    // Observer Implementation
    void onGameStateChanged(GameState newState) override;
    void onRobotMoved(Point p) override;
    void onPathFound(const std::vector<Point>& path) override;
    void onLevelUp(int newLevel) override;
    void onSolvingComplete(const ScoreData& score) override;
};





