// FileManager.cpp
#include "FileManager.hpp"
#include <fstream>

bool FileManager::readLines(const std::string& path, std::vector<std::string>& out) {
    std::ifstream f(path);
    if (!f.is_open()) return false;

    std::string line;
    while (std::getline(f, line))
        out.push_back(line);

    return true;
}