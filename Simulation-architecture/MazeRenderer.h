#pragma once
#include <SFML/Graphics.hpp>
#include "Maze.h"

class MazeRenderer {
private:
    const float CELL_SIZE = 40.0f;
public:
    void draw(sf::RenderWindow& window, const Maze& maze);
};


