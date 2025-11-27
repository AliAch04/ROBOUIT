#include "LevelSystem.h"

LevelSystem::LevelSystem() {
    experienceTable[1] = 100;
    experienceTable[2] = 250;
    experienceTable[3] = 500;
    experienceTable[4] = 1000;
}

LevelSystem& LevelSystem::getInstance() {
    static LevelSystem instance;
    return instance;
}

int LevelSystem::getExperienceRequired(int level) const {
    if (experienceTable.count(level)) return experienceTable.at(level);
    return 999999;
}

bool LevelSystem::canLevelUp(int currentXp, int currentLevel) const {
    return currentXp >= getExperienceRequired(currentLevel);
}

