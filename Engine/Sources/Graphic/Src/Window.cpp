#include "Window.h"
#include "Input/InputSystem.h"

#include <glfw/glfw3.h>

#include <stdexcept>


namespace TE {

Window::Window(const std::string& appName, int width, int height) : _glfwWindow(nullptr), _width(width), _height(height)
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

Window::~Window()
{
    glfwDestroyWindow(_glfwWindow);
}

glm::ivec2 Window::GetFramebufferSize()
{
    glm::ivec2 size;
    glfwGetFramebufferSize(_glfwWindow, &size.x, &size.y);

    return size;
}

void Window::RegisterInput(const InputSystem& inputSystem)
{
    glfwSetInputMode(_glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetCursorPosCallback(_glfwWindow, InputSystem::cursor_position_callback);
    glfwSetKeyCallback(_glfwWindow, InputSystem::key_callback);
}

void Window::UnregisterInput(const InputSystem& inputSystem)
{
}

bool Window::ShouldClose()
{
    return glfwWindowShouldClose(_glfwWindow);
}

GLFWwindow* Window::GetRawWindow()
{
    return _glfwWindow;
}

}