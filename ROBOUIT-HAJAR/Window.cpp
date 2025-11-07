// Window.cpp
#include "Window.hpp"

Window::Window(int w, int h, const std::string& title)
    : width(w), height(h), title(title)
{
    // Later: create SFML window
}

bool Window::isOpen() const {
    // Later: return SFMLwindow.isOpen();
    return true;
}

//void Window::pollEvents() {
//    // Later: handle events
//}

void Window::clear() { /* later */ }
void Window::display() { /* later */ }