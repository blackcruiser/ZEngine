#include "Window.h"

#include <glfw/glfw3.h>

#include <stdexcept>

TEWindow::TEWindow(const std::string &appName, int width, int height) : _glfwWindow(nullptr), _width(width), _height(height)
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    _glfwWindow = glfwCreateWindow(width, height, appName.c_str(), nullptr, nullptr);

    if (_glfwWindow == nullptr)
    {
        throw std::runtime_error("failed to create window!");
        return;
    }
}

TEWindow::~TEWindow()
{
    glfwDestroyWindow(_glfwWindow);
}

bool TEWindow::ShouldClose()
{
    return glfwWindowShouldClose(_glfwWindow);
}

GLFWwindow *TEWindow::GetRawWindow()
{
    return _glfwWindow;
}

glm::ivec2 TEWindow::GetFramebufferSize()
{
    glm::ivec2 size;
    glfwGetFramebufferSize(_glfwWindow, &size.x, &size.y);

    return size;
}