#include "Window.h"
#include "Input/InputSystem.h"
#include "Graphic/VulkanDevice.h"
#include "Viewport.h"

#include <glfw/glfw3.h>
#include <vulkan/vulkan.h>

#include <stdexcept>


namespace ZE {

Window::Window(const std::string& title, const glm::ivec2& size)
    : _size(size), _glfwWindow(nullptr)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    _glfwWindow = glfwCreateWindow(size.x, size.y, title.c_str(), nullptr, nullptr);

    if (_glfwWindow == nullptr)
    {
        throw std::runtime_error("failed to create window!");
        return;
    }

    glfwSetInputMode(_glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

Window::~Window()
{
    glfwDestroyWindow(_glfwWindow);
    glfwTerminate();
}

bool Window::ShouldClose()
{
    return glfwWindowShouldClose(_glfwWindow);
}

void Window::CreateViewport(VulkanDevice* device)
{
    _viewport = new Viewport(_glfwWindow, _size);
}

Viewport* Window::GetViewport()
{
    return _viewport;
}

glm::ivec2 Window::GetSize()
{
    glm::ivec2 size;
    glfwGetFramebufferSize(_glfwWindow, &size.x, &size.y);

    return size;
}

void MouseButtonFunc(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    {
        int mode = glfwGetInputMode(window, GLFW_CURSOR);
        
        if (mode == GLFW_CURSOR_DISABLED)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}

void Window::RegisterInput(const InputSystem& inputSystem)
{
    glfwSetCursorPosCallback(_glfwWindow, InputSystem::cursor_position_callback);
    //glfwSetKeyCallback(_glfwWindow, InputSystem::key_callback);
    glfwSetMouseButtonCallback(_glfwWindow, &MouseButtonFunc);
}

void Window::UnregisterInput(const InputSystem& inputSystem)
{
}

} // namespace ZE