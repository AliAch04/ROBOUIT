#include "PathFinder.h"
#include "ManhattanHeuristic.h"
#include <queue>
#include <unordered_set>
#include <algorithm>

struct NodeCompare {
    bool operator()(const Node* a, const Node* b) {
        return a->getFCost() > b->getFCost();
    }
};

PathFinder::PathFinder() {
    heuristic = std::make_unique<ManhattanHeuristic>();
}

PathFinder::~PathFinder() { clearMemory(); }

void PathFinder::clearMemory() {
    for (Node* node : nodeMemory) delete node;
    nodeMemory.clear();
}

std::vector<Point> PathFinder::findPath(Maze* maze) {
    clearMemory();
    std::priority_queue<Node*, std::vector<Node*>, NodeCompare> openList;
    std::unordered_set<Point, PointHash> closedList;

    Node* startNode = new Node(maze->startPos, 0, heuristic->calculate(maze->startPos, maze->endPos));
    openList.push(startNode);
    nodeMemory.push_back(startNode);

    Node* endNode = nullptr;

    while (!openList.empty()) {
        Node* current = openList.top();
        openList.pop();

        if (current->position == maze->endPos) { endNode = current; break; }
        if (closedList.count(current->position)) continue;
        closedList.insert(current->position);

        Point dirs[4] = { {0,1}, {0,-1}, {1,0}, {-1,0} };
        for (Point d : dirs) {
            Point neighborPos = { current->position.x + d.x, current->position.y + d.y };
            if (!maze->isValid(neighborPos) || maze->isWall(neighborPos)) continue;
            if (closedList.count(neighborPos)) continue;

            float newG = current->gCost + 1.0f;
            float newH = heuristic->calculate(neighborPos, maze->endPos);
            Node* neighborNode = new Node(neighborPos, newG, newH, current);

            openList.push(neighborNode);
            nodeMemory.push_back(neighborNode);
        }
    }

    std::vector<Point> path;
    Node* temp = endNode;
    while (temp) {
        path.push_back(temp->position);
        temp = temp->parent;
    }
    std::reverse(path.begin(), path.end());
    return path;
}




