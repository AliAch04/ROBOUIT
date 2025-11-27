#include "FileManager.h"
#include <iostream>
#include <fstream>

bool FileManager::saveRobot(const Robot& robot, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) return false;

    // Simple JSON-like text format
    file << "{\n";
    file << "  \"level\": " << robot.getLevel() << ",\n";
    file << "  \"status\": \"SAVED\"\n";
    file << "}\n";

    std::cout << "[Persistence] Game Saved to " << filename << std::endl;
    return true;
}

