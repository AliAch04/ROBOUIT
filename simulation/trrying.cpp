#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <stack>
#include <random>
#include <chrono>
#include <queue>
#include <unordered_map>
#include <cmath>
#include <string>
#include <optional>
#include <set>
#include <sstream>

// -----------------------------------------------------------------------------
// Utility types
// -----------------------------------------------------------------------------
struct Vec2i {
    int x, y;
};

inline bool operator==(Vec2i a, Vec2i b) { return a.x == b.x && a.y == b.y; }

struct Vec2iHash {
    size_t operator()(Vec2i const& v) const noexcept {
        return std::hash<int>()(v.x) ^ (std::hash<int>()(v.y) << 1);
    }
};

// -----------------------------------------------------------------------------
// Maze generation (recursive backtracker)
// Cell bitmask: 1 = top, 2 = right, 4 = bottom, 8 = left
// -----------------------------------------------------------------------------
class Maze {
public:
    int cols, rows;
    std::vector<int> cells;

    Maze() : cols(0), rows(0) {}
    Maze(int c, int r) { generate(c, r); }

    void generate(int c, int r) {
        cols = c;
        rows = r;
        cells.assign(cols * rows, 1 | 2 | 4 | 8);

        std::mt19937 rng((unsigned)std::chrono::system_clock::now().time_since_epoch().count());
        std::stack<Vec2i> st;
        auto idx = [&](int x, int y) { return y * cols + x; };
        std::vector<char> visited(cols * rows, 0);

        int sx = 0, sy = 0;
        visited[idx(sx, sy)] = 1;
        st.push({ sx, sy });

        while (!st.empty()) {
            Vec2i v = st.top();
            int x = v.x, y = v.y;

            std::vector<Vec2i> nbs;
            if (y > 0 && !visited[idx(x, y - 1)]) nbs.push_back({ x, y - 1 });
            if (x + 1 < cols && !visited[idx(x + 1, y)]) nbs.push_back({ x + 1, y });
            if (y + 1 < rows && !visited[idx(x, y + 1)]) nbs.push_back({ x, y + 1 });
            if (x > 0 && !visited[idx(x - 1, y)]) nbs.push_back({ x - 1, y });

            if (!nbs.empty()) {
                std::uniform_int_distribution<int> d(0, (int)nbs.size() - 1);
                Vec2i nb = nbs[d(rng)];

                if (nb.x == x && nb.y == y - 1) { // up
                    cells[idx(x, y)] &= ~1; cells[idx(nb.x, nb.y)] &= ~4;
                }
                else if (nb.x == x + 1 && nb.y == y) { // right
                    cells[idx(x, y)] &= ~2; cells[idx(nb.x, nb.y)] &= ~8;
                }
                else if (nb.x == x && nb.y == y + 1) { // down
                    cells[idx(x, y)] &= ~4; cells[idx(nb.x, nb.y)] &= ~1;
                }
                else if (nb.x == x - 1 && nb.y == y) { // left
                    cells[idx(x, y)] &= ~8; cells[idx(nb.x, nb.y)] &= ~2;
                }

                visited[idx(nb.x, nb.y)] = 1;
                st.push(nb);
            }
            else {
                st.pop();
            }
        }
    }

    bool hasWall(int x, int y, int dirBit) const {
        return (cells[y * cols + x] & dirBit) != 0;
    }
};

// -----------------------------------------------------------------------------
// Pathfinding (A* and Dijkstra)
// -----------------------------------------------------------------------------
struct Node {
    Vec2i pos;
    float g = 0, h = 0;
    float f() const { return g + h; }
    Vec2i parent{ -1, -1 };
};

// ---------- A* ----------
std::optional<std::vector<Vec2i>> astar(const Maze& maze, Vec2i start, Vec2i goal) {
    int W = maze.cols, H = maze.rows;
    auto inb = [&](int x, int y) { return x >= 0 && y >= 0 && x < W && y < H; };
    auto heuristic = [&](Vec2i a, Vec2i b) { return float(std::abs(a.x - b.x) + std::abs(a.y - b.y)); };
    std::unordered_map<int, Node> nodes;
    auto key = [&](int x, int y) { return y * W + x; };

    auto pushNode = [&](int x, int y, float g, Vec2i parent) {
        int k = key(x, y);
        Node n;
        n.pos = { x, y };
        n.g = g;
        n.h = heuristic({ x, y }, goal);
        n.parent = parent;
        nodes[k] = n;
        };

    struct OpenEntry { float f; int k; };
    struct Cmp { bool operator()(OpenEntry a, OpenEntry b) const { return a.f > b.f; } };
    std::priority_queue<OpenEntry, std::vector<OpenEntry>, Cmp> open;

    pushNode(start.x, start.y, 0, { -1, -1 });
    open.push({ nodes[key(start.x, start.y)].f(), key(start.x, start.y) });

    std::set<int> closed;

    while (!open.empty()) {
        auto curk = open.top().k;
        open.pop();
        if (closed.count(curk)) continue;
        closed.insert(curk);

        Node cur = nodes[curk];
        if (cur.pos == goal) {
            std::vector<Vec2i> path;
            Vec2i p = cur.pos;
            while (!(p.x == -1 && p.y == -1)) {
                path.push_back(p);
                int k = key(p.x, p.y);
                p = nodes[k].parent;
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        int x = cur.pos.x, y = cur.pos.y;
        std::vector<std::pair<Vec2i, float>> neighbors;

        if (!maze.hasWall(x, y, 1) && inb(x, y - 1)) neighbors.push_back({ {x, y - 1}, 1.0f });
        if (!maze.hasWall(x, y, 2) && inb(x + 1, y)) neighbors.push_back({ {x + 1, y}, 1.0f });
        if (!maze.hasWall(x, y, 4) && inb(x, y + 1)) neighbors.push_back({ {x, y + 1}, 1.0f });
        if (!maze.hasWall(x, y, 8) && inb(x - 1, y)) neighbors.push_back({ {x - 1, y}, 1.0f });

        for (auto& nb : neighbors) {
            int nk = key(nb.first.x, nb.first.y);
            if (closed.count(nk)) continue;
            float tentative_g = cur.g + nb.second;
            if (nodes.find(nk) == nodes.end() || tentative_g < nodes[nk].g) {
                pushNode(nb.first.x, nb.first.y, tentative_g, cur.pos);
                open.push({ nodes[nk].f(), nk });
            }
        }
    }

    return std::nullopt;
}

// ---------- Dijkstra ----------
std::optional<std::vector<Vec2i>> dijkstra(const Maze& maze, Vec2i start, Vec2i goal) {
    int W = maze.cols, H = maze.rows;
    auto inb = [&](int x, int y) { return x >= 0 && y >= 0 && x < W && y < H; };
    std::unordered_map<int, Node> nodes;
    auto key = [&](int x, int y) { return y * W + x; };

    auto pushNode = [&](int x, int y, float g, Vec2i parent) {
        int k = key(x, y);
        Node n;
        n.pos = { x, y };
        n.g = g;
        n.h = 0;
        n.parent = parent;
        nodes[k] = n;
        };

    struct OpenEntry { float f; int k; };
    struct Cmp { bool operator()(OpenEntry a, OpenEntry b) const { return a.f > b.f; } };
    std::priority_queue<OpenEntry, std::vector<OpenEntry>, Cmp> open;
    std::set<int> closed;

    pushNode(start.x, start.y, 0, { -1, -1 });
    open.push({ nodes[key(start.x, start.y)].f(), key(start.x, start.y) });

    while (!open.empty()) {
        auto curk = open.top().k;
        open.pop();
        if (closed.count(curk)) continue;
        closed.insert(curk);

        Node cur = nodes[curk];
        if (cur.pos == goal) {
            std::vector<Vec2i> path;
            Vec2i p = cur.pos;
            while (!(p.x == -1 && p.y == -1)) {
                path.push_back(p);
                int k = key(p.x, p.y);
                p = nodes[k].parent;
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        int x = cur.pos.x, y = cur.pos.y;
        std::vector<std::pair<Vec2i, float>> neighbors;

        if (!maze.hasWall(x, y, 1) && inb(x, y - 1)) neighbors.push_back({ {x, y - 1}, 1.0f });
        if (!maze.hasWall(x, y, 2) && inb(x + 1, y)) neighbors.push_back({ {x + 1, y}, 1.0f });
        if (!maze.hasWall(x, y, 4) && inb(x, y + 1)) neighbors.push_back({ {x, y + 1}, 1.0f });
        if (!maze.hasWall(x, y, 8) && inb(x - 1, y)) neighbors.push_back({ {x - 1, y}, 1.0f });

        for (auto& nb : neighbors) {
            int nk = key(nb.first.x, nb.first.y);
            if (closed.count(nk)) continue;
            float tentative_g = cur.g + nb.second;
            if (nodes.find(nk) == nodes.end() || tentative_g < nodes[nk].g) {
                pushNode(nb.first.x, nb.first.y, tentative_g, cur.pos);
                open.push({ nodes[nk].f(), nk });
            }
        }
    }

    return std::nullopt;
}

// -----------------------------------------------------------------------------
// Game class
// -----------------------------------------------------------------------------
enum class Screen { Home, Levels, Options, Playing, Victory };
struct LevelSpec { int cols, rows; };

class Game {
public:
    sf::RenderWindow window;
    Screen screen = Screen::Home;
    sf::Font font;
    bool soundOn = true;
    int currentLevel = 0;
    std::vector<LevelSpec> levels{ {15,11}, {25,17}, {41,25} };

    Maze maze;
    std::vector<Vec2i> path;
    Vec2i startCell{ 0,0 }, goalCell{ 0,0 };
    bool useAStar = true;
    bool autoNavigate = true;
    Vec2i aiPos{ 0,0 };
    size_t aiPathIndex = 0;
    float cellSize = 24.0f;
    float padding = 20.0f;
    sf::SoundBuffer clickBuf;
    sf::Sound clickSound;

    Game() : window(sf::VideoMode(1024, 768), "Labyrinth AI - SFML") {
        window.setFramerateLimit(60);
        font.loadFromFile("C:/Windows/Fonts/arial.ttf");
        clickBuf.loadFromFile("click.wav");
        clickSound.setBuffer(clickBuf);
    }

    void startLevel(int idx);
    void computePath();
    void update(float dt);
    void drawText(const std::string& s, float x, float y, int size = 20, sf::Color col = sf::Color::White);
    void drawHome();
    void drawLevels();
    void drawOptions();
    void drawMaze();
    void drawPlaying();
    void run();
    void handleEvents();
    void handleMouseClick(int mx, int my);
};

// -----------------------------------------------------------------------------
// Définition des fonctions de la classe Game
// -----------------------------------------------------------------------------

void Game::startLevel(int idx) {
    currentLevel = idx;
    maze.generate(levels[idx].cols, levels[idx].rows);
    startCell = { 0, 0 };
    goalCell = { maze.cols - 1, maze.rows - 1 };
    aiPos = startCell;
    aiPathIndex = 0;
    computePath();
    screen = Screen::Playing;
}

void Game::computePath() {
    if (useAStar)
        path = astar(maze, startCell, goalCell).value_or(std::vector<Vec2i>{});
    else
        path = dijkstra(maze, startCell, goalCell).value_or(std::vector<Vec2i>{});
}

void Game::update(float dt) {
    if (screen == Screen::Playing && autoNavigate && !path.empty()) {
        if (aiPathIndex + 1 < path.size()) {
            sf::Vector2f current(aiPos.x, aiPos.y);
            sf::Vector2f next(path[aiPathIndex + 1].x, path[aiPathIndex + 1].y);
            if (current == next)
                aiPathIndex++;
            else {
                aiPos = path[aiPathIndex + 1];
                aiPathIndex++;
            }
        }
        else {
            screen = Screen::Victory;
        }
    }
}

void Game::drawText(const std::string& s, float x, float y, int size, sf::Color col) {
    sf::Text txt(s, font, size);
    txt.setPosition(x, y);
    txt.setFillColor(col);
    window.draw(txt);
}

void Game::drawHome() {
    window.clear(sf::Color(40, 40, 60));
    drawText("🏁 LABYRINTH AI", 320, 150, 48, sf::Color::Yellow);
    drawText("1. Start Game", 420, 300, 28);
    drawText("2. Options", 420, 350, 28);
    drawText("3. Quit", 420, 400, 28);
}

void Game::drawLevels() {
    window.clear(sf::Color(30, 50, 70));
    drawText("Choose a Level", 380, 100, 36, sf::Color::Cyan);
    for (int i = 0; i < (int)levels.size(); ++i) {
        std::stringstream ss;
        ss << "Level " << (i + 1) << " (" << levels[i].cols << "x" << levels[i].rows << ")";
        drawText(ss.str(), 400, 220 + i * 60, 28);
    }
}

void Game::drawOptions() {
    window.clear(sf::Color(60, 40, 60));
    drawText("Options", 420, 120, 40, sf::Color::Green);
    drawText(std::string("Algorithm: ") + (useAStar ? "A*" : "Dijkstra"), 380, 260, 28);
    drawText(std::string("Auto Move: ") + (autoNavigate ? "ON" : "OFF"), 380, 300, 28);
    drawText(std::string("Sound: ") + (soundOn ? "ON" : "OFF"), 380, 340, 28);
    drawText("Press ESC to return", 380, 460, 20, sf::Color(180, 180, 180));
}

void Game::drawMaze() {
    float offsetX = padding;
    float offsetY = padding + 60;
    sf::RectangleShape wall(sf::Vector2f(cellSize, 2));
    wall.setFillColor(sf::Color::White);

    // Dessin du labyrinthe
    for (int y = 0; y < maze.rows; ++y) {
        for (int x = 0; x < maze.cols; ++x) {
            float px = offsetX + x * cellSize;
            float py = offsetY + y * cellSize;
            int c = maze.cells[y * maze.cols + x];
            if (c & 1) { wall.setSize({ cellSize, 2 }); wall.setPosition(px, py); window.draw(wall); }
            if (c & 2) { wall.setSize({ 2, cellSize }); wall.setPosition(px + cellSize, py); window.draw(wall); }
            if (c & 4) { wall.setSize({ cellSize, 2 }); wall.setPosition(px, py + cellSize); window.draw(wall); }
            if (c & 8) { wall.setSize({ 2, cellSize }); wall.setPosition(px, py); window.draw(wall); }
        }
    }

    // Start
    sf::CircleShape startCircle(cellSize / 3);
    startCircle.setFillColor(sf::Color::Green);
    startCircle.setPosition(offsetX + startCell.x * cellSize + cellSize / 6,
        offsetY + startCell.y * cellSize + cellSize / 6);
    window.draw(startCircle);

    // Goal
    sf::CircleShape goalCircle(cellSize / 3);
    goalCircle.setFillColor(sf::Color::Red);
    goalCircle.setPosition(offsetX + goalCell.x * cellSize + cellSize / 6,
        offsetY + goalCell.y * cellSize + cellSize / 6);
    window.draw(goalCircle);

    // Path
    sf::RectangleShape pathRect(sf::Vector2f(cellSize / 2, cellSize / 2));
    pathRect.setFillColor(sf::Color(0, 100, 255, 120));
    for (auto& p : path) {
        pathRect.setPosition(offsetX + p.x * cellSize + cellSize / 4,
            offsetY + p.y * cellSize + cellSize / 4);
        window.draw(pathRect);
    }

    // AI
    sf::CircleShape aiCircle(cellSize / 3);
    aiCircle.setFillColor(sf::Color::Yellow);
    aiCircle.setPosition(offsetX + aiPos.x * cellSize + cellSize / 6,
        offsetY + aiPos.y * cellSize + cellSize / 6);
    window.draw(aiCircle);
}

void Game::drawPlaying() {
    window.clear(sf::Color(20, 20, 40));
    drawMaze();
    drawText("Press ESC to quit to home", 20, 10, 20, sf::Color(180, 180, 180));
}

void Game::handleMouseClick(int mx, int my) {
    if (screen == Screen::Home) {
        if (my > 280 && my < 330) { if (soundOn) clickSound.play(); screen = Screen::Levels; }
        else if (my > 330 && my < 380) { if (soundOn) clickSound.play(); screen = Screen::Options; }
        else if (my > 380 && my < 440) window.close();
    }
    else if (screen == Screen::Levels) {
        for (int i = 0; i < (int)levels.size(); ++i) {
            if (my > 220 + i * 60 && my < 260 + i * 60) {
                if (soundOn) clickSound.play();
                startLevel(i);
            }
        }
    }
    else if (screen == Screen::Options) {
        if (my > 250 && my < 290) useAStar = !useAStar;
        else if (my > 290 && my < 330) autoNavigate = !autoNavigate;
        else if (my > 330 && my < 370) soundOn = !soundOn;
    }
    else if (screen == Screen::Victory) {
        screen = Screen::Home;
    }
}

void Game::handleEvents() {
    sf::Event e;
    while (window.pollEvent(e)) {
        if (e.type == sf::Event::Closed) window.close();
        else if (e.type == sf::Event::KeyPressed) {
            if (e.key.code == sf::Keyboard::Escape) screen = Screen::Home;
        }
        else if (e.type == sf::Event::MouseButtonPressed) {
            handleMouseClick(e.mouseButton.x, e.mouseButton.y);
        }
    }
}

void Game::run() {
    sf::Clock clock;
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        handleEvents();
        update(dt);

        if (screen == Screen::Home) drawHome();
        else if (screen == Screen::Levels) drawLevels();
        else if (screen == Screen::Options) drawOptions();
        else if (screen == Screen::Playing) drawPlaying();
        else if (screen == Screen::Victory) {
            window.clear(sf::Color(20, 60, 20));
            drawText("🎉 LEVEL COMPLETE! 🎉", 300, 300, 40, sf::Color::Yellow);
            drawText("Click to return", 420, 400, 24);
        }

        window.display();
    }
}

// -----------------------------------------------------------------------------
// Main
// -----------------------------------------------------------------------------
/*
int main() {
    Game g;
    g.run();
    return 0;
}

*/








/*
#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(shape);
        window.display();
    }

    return 0;
}
*/


/*
#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Network.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>

//using namespace sf;

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "My First Game", sf::Style::Titlebar | sf::Style::Close);
    sf::Event ev;

    //Game Loop
    while (window.isOpen())
    {
        //Event pooling
        while (window.pollEvent(ev))
        {
            switch (ev.type)
            {
            case sf::Event::Closed:
                window.close();
                break;

            case sf::Event::KeyPressed:
                if (ev.key.code == sf::Keyboard::Escape)
                    window.close();
                break;
            }
        }

        //Update


        //Render
        window.clear(sf::Color::Blue); //Clear old frame

        //Draw your game here



        window.display(); //Tell app that window is done drawing

    }

    //End Of Application
    return 0;
}
*/


/*
#include <SFML/Graphics.hpp>
#include <vector>
#include <queue>
#include <cmath>
#include <algorithm>
#include <iostream>

// Simple 10x10 maze: 0=free, 1=wall
int mazeData[10][10] = {
    {0,0,0,1,0,0,0,0,0,0},
    {1,1,0,1,0,1,1,1,1,0},
    {0,0,0,0,0,0,0,0,1,0},
    {0,1,1,1,1,1,1,0,1,0},
    {0,0,0,0,0,0,1,0,1,0},
    {1,1,1,1,1,0,1,0,0,0},
    {0,0,0,0,0,0,1,1,1,0},
    {0,1,1,1,1,0,0,0,1,0},
    {0,0,0,0,0,0,1,0,1,0},
    {1,1,1,1,1,0,0,0,0,0}
};

struct Vec2i { int x, y; };

struct Node {
    Vec2i pos;
    float g, h;
    Node* parent;
    float f() const { return g + h; }
};

// A* algorithm
std::vector<Vec2i> AStar(const std::vector<std::vector<int>>& maze, Vec2i start, Vec2i goal) {
    int rows = maze.size();
    int cols = maze[0].size();
    std::vector<Vec2i> dirs = { {1,0},{-1,0},{0,1},{0,-1} };

    auto heuristic = [](Vec2i a, Vec2i b) { return float(abs(a.x - b.x) + abs(a.y - b.y)); };

    std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));
    std::priority_queue<std::pair<float, Node*>, std::vector<std::pair<float, Node*>>, std::greater<>> openSet;

    Node* startNode = new Node{ start,0,heuristic(start,goal),nullptr };
    openSet.push({ startNode->f(), startNode });
    Node* current = nullptr;

    while (!openSet.empty()) {
        current = openSet.top().second;
        openSet.pop();

        if (current->pos.x == goal.x && current->pos.y == goal.y) break;
        visited[current->pos.y][current->pos.x] = true;

        for (auto d : dirs) {
            Vec2i nb = { current->pos.x + d.x, current->pos.y + d.y };
            if (nb.x < 0 || nb.y < 0 || nb.x >= cols || nb.y >= rows) continue;
            if (maze[nb.y][nb.x] == 1 || visited[nb.y][nb.x]) continue;
            Node* next = new Node{ nb, current->g + 1, heuristic(nb,goal), current };
            openSet.push({ next->f(), next });
        }
    }

    // Reconstruct path
    std::vector<Vec2i> path;
    while (current != nullptr) {
        path.push_back(current->pos);
        current = current->parent;
    }
    std::reverse(path.begin(), path.end());
    return path;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(500, 500), "Simple Maze AI");
    int rows = 10, cols = 10;
    float cellSize = 50.0f;

    std::vector<std::vector<int>> maze(rows, std::vector<int>(cols, 0));
    for (int y = 0; y < rows; y++)
        for (int x = 0; x < cols; x++)
            maze[y][x] = mazeData[y][x];

    Vec2i start{ 0,0 };
    Vec2i goal{ 9,9 };

    std::vector<Vec2i> path = AStar(maze, start, goal);
    int aiIndex = 0;
    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event ev;
        while (window.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed) window.close();
        }

        float dt = clock.restart().asSeconds();
        // AI movement: step every 0.3s
        static float t = 0; t += dt;
        if (t > 0.3f && aiIndex + 1 < path.size()) { aiIndex++; t = 0; }

        window.clear(sf::Color::Black);

        // Draw maze
        for (int y = 0; y < rows; y++) {
            for (int x = 0; x < cols; x++) {
                sf::RectangleShape rect(sf::Vector2f(cellSize - 2, cellSize - 2));
                rect.setPosition(x * cellSize + 1, y * cellSize + 1);
                rect.setFillColor(maze[y][x] == 1 ? sf::Color::White : sf::Color(50, 50, 50));
                window.draw(rect);
            }
        }

        // Draw path (optional)
        for (auto p : path) {
            sf::RectangleShape rect(sf::Vector2f(cellSize * 0.6f, cellSize * 0.6f));
            rect.setOrigin(cellSize * 0.3f, cellSize * 0.3f);
            rect.setPosition(p.x * cellSize + cellSize / 2, p.y * cellSize + cellSize / 2);
            rect.setFillColor(sf::Color(0, 255, 0, 100));
            window.draw(rect);
        }

        // Draw AI
        sf::CircleShape ai(cellSize * 0.3f);
        ai.setOrigin(cellSize * 0.3f, cellSize * 0.3f);
        ai.setPosition(path[aiIndex].x * cellSize + cellSize / 2, path[aiIndex].y * cellSize + cellSize / 2);
        ai.setFillColor(sf::Color::Blue);
        window.draw(ai);

        // Start & goal
        sf::RectangleShape startRect(sf::Vector2f(cellSize * 0.6f, cellSize * 0.6f));
        startRect.setOrigin(cellSize * 0.3f, cellSize * 0.3f);
        startRect.setPosition(start.x * cellSize + cellSize / 2, start.y * cellSize + cellSize / 2);
        startRect.setFillColor(sf::Color::Green);
        window.draw(startRect);

        sf::RectangleShape goalRect(sf::Vector2f(cellSize * 0.6f, cellSize * 0.6f));
        goalRect.setOrigin(cellSize * 0.3f, cellSize * 0.3f);
        goalRect.setPosition(goal.x * cellSize + cellSize / 2, goal.y * cellSize + cellSize / 2);
        goalRect.setFillColor(sf::Color::Red);
        window.draw(goalRect);

        window.display();
    }
}
*/
