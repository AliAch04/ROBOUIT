#pragma once
#include <string>
#include "Robot.h"

class FileManager {
public:
    bool saveRobot(const Robot& robot, const std::string& filename);
};