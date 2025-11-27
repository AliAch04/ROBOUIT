#pragma once

#include <memory>
#include "Point.h"
#include "Enums.h"

class Cell {
protected:
    Point position;
    CellType type;
public:
    Cell(Point pos, CellType t);
    virtual ~Cell() = default;
    virtual bool isWalkable() const { return true; }
    CellType getType() const { return type; }
    Point getPosition() const { return position; }
    static std::unique_ptr<Cell> create(CellType type, Point pos);
};

class WallCell : public Cell {
public:
    WallCell(Point pos) : Cell(pos, CellType::WALL) {}
    bool isWalkable() const override { return false; }
};
class EmptyCell : public Cell { public: EmptyCell(Point pos) : Cell(pos, CellType::EMPTY) {} };
class StartCell : public Cell { public: StartCell(Point pos) : Cell(pos, CellType::START) {} };
class EndCell : public Cell { public: EndCell(Point pos) : Cell(pos, CellType::END) {} };




