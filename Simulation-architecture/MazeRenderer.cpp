#include "MazeRenderer.h"

void MazeRenderer::draw(sf::RenderWindow& window, const Maze& maze) {
    sf::RectangleShape cell(sf::Vector2f(CELL_SIZE - 2, CELL_SIZE - 2));

    for (int y = 0; y < maze.height; ++y) {
        for (int x = 0; x < maze.width; ++x) {
            // Pas de vérification nullptr ici pour simplifier, mais recommandé en prod
            CellType t = maze.grid[y][x]->getType();
            cell.setPosition(x * CELL_SIZE + 1, y * CELL_SIZE + 1);

            switch (t) {
            case CellType::WALL: cell.setFillColor(sf::Color::Black); break;
            case CellType::START: cell.setFillColor(sf::Color::Green); break;
            case CellType::END: cell.setFillColor(sf::Color::Red); break;
            default: cell.setFillColor(sf::Color(200, 200, 200)); break;
            }
            window.draw(cell);
        }
    }
}


