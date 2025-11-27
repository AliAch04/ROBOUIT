#pragma once
#include "Point.h"
#include "Enums.h"

class Robot {
private:
    Point currentPos;
    RobotState state;
    int level;
    int experience;

public:
    Robot();
    void setPosition(Point p);
    Point getPosition() const;
    void setState(RobotState s);
    RobotState getState() const;
    void addExperience(int points);
    int getLevel() const;
};


