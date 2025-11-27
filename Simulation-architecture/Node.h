#pragma once

#pragma once
#include "Point.h"

struct Node {
    Point position;
    float gCost;
    float hCost;
    Node* parent;

    Node(Point pos, float g, float h, Node* p = nullptr)
        : position(pos), gCost(g), hCost(h), parent(p) {
    }

    float getFCost() const { return gCost + hCost; }
};


