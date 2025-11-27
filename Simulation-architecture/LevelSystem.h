#pragma once
#include <map>

class LevelSystem {
private:
    std::map<int, int> experienceTable;
    LevelSystem(); // Singleton private constructor
public:
    static LevelSystem& getInstance();
    int getExperienceRequired(int level) const;
    bool canLevelUp(int currentXp, int currentLevel) const;
};


