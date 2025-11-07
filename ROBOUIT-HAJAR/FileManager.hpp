// FileManager.hpp
#pragma once
#include <vector>
#include <string>

class FileManager {
public:
    static bool readLines(const std::string& path, std::vector<std::string>& out);
};