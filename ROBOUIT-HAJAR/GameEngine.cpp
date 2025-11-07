// GameEngine.cpp
#include "GameEngine.hpp"

GameEngine::GameEngine() : window(nullptr) {}

bool GameEngine::init() {
    window = new Window(800, 600, "Maze AI");

    std::vector<std::string> lines;
    FileManager::readLines("maze.txt", lines);
    maze.load(lines);

    return true;
}

void GameEngine::run() {
    while (window->isOpen()) {
        window->pollEvents();
        window->clear();

        // Later: draw maze
        // Later: AI updates

        window->display();
    }
}