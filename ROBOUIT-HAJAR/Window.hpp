// Window.hpp
#pragma once
#include <string>

class Window {
public:
    Window(int w, int h, const std::string& title);

    //void pollEvents();
    bool isOpen() const;
    void clear();
    void display();

private:
    int width, height;
    std::string title;

    // Add SFML/SDL window object later
};