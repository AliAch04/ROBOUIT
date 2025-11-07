// GameEngine.hpp
#pragma once
#include "Window.hpp"
#include "Maze.hpp"
#include "FileManager.hpp"

class GameEngine {
public:
    GameEngine();

    bool init();
    void run();

private:
    Window* window;
    Maze maze;
};