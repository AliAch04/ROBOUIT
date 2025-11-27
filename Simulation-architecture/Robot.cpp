#include "Robot.h"
#include "LevelSystem.h"

Robot::Robot() : currentPos{ 0,0 }, state(RobotState::IDLE), level(1), experience(0) {}
void Robot::setPosition(Point p) { currentPos = p; }
Point Robot::getPosition() const { return currentPos; }
void Robot::setState(RobotState s) { state = s; }
RobotState Robot::getState() const { return state; }

void Robot::addExperience(int points) {
    experience += points;
    if (LevelSystem::getInstance().canLevelUp(experience, level)) {
        level++;
        // Notification logic handled by GameEngine usually
    }
}
int Robot::getLevel() const { return level; }




