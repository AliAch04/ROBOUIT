#pragma once
#include <vector>
#include <memory>
#include "Maze.h"
#include "Node.h"
#include "IHeuristic.h"

class PathFinder {
private:
    std::unique_ptr<IHeuristic> heuristic;
    std::vector<Node*> nodeMemory;

public:
    PathFinder();
    ~PathFinder();
    std::vector<Point> findPath(Maze* maze);
    void clearMemory();
};


