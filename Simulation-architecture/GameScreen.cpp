#include "GameScreen.h"
#include <iostream>

GameScreen::GameScreen()
    : window(sf::VideoMode(400, 400), "Robot Maze Architecture"),
    robotShape(15.f)
{
    robotShape.setFillColor(sf::Color::Blue);
    robotShape.setOrigin(15.f, 15.f);

    engine.addObserver(this);
    engine.loadLevel();
}

void GameScreen::run() {
    engine.startSolving(); // Auto-start for this demo

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
        }

        sf::Time dt = deltaClock.restart();
        engine.update(dt.asSeconds());

        window.clear(sf::Color(50, 50, 50));

        // Draw components
        mazeRenderer.draw(window, engine.getMaze());

        for (const auto& rect : pathShapes) {
            window.draw(rect);
        }

        window.draw(robotShape);
        window.display();
    }
}

// --- Observer Callbacks ---

void GameScreen::onGameStateChanged(GameState newState) {
    if (newState == GameState::COMPLETE) std::cout << "UI: Level Complete!" << std::endl;
}

void GameScreen::onRobotMoved(Point p) {
    // Convert grid coordinate to pixel coordinate
    float pixelX = p.x * 40.0f + 20.0f;
    float pixelY = p.y * 40.0f + 20.0f;
    robotShape.setPosition(pixelX, pixelY);
}

void GameScreen::onPathFound(const std::vector<Point>& path) {
    pathShapes.clear();
    for (const auto& p : path) {
        sf::RectangleShape rect(sf::Vector2f(10.f, 10.f));
        rect.setFillColor(sf::Color::Yellow);
        rect.setPosition(p.x * 40.0f + 15.0f, p.y * 40.0f + 15.0f);
        pathShapes.push_back(rect);
    }
}

void GameScreen::onLevelUp(int newLevel) {
    std::cout << "UI: LEVEL UP! New Level: " << newLevel << std::endl;
}

void GameScreen::onSolvingComplete(const ScoreData& score) {
    std::cout << "UI: Final Score: " << score.totalScore << " XP Gained: " << score.experienceGained << std::endl;
}


