#pragma once

#include "CoreDefines.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <string>


namespace TE {

class InputSystem;

class Window
{
public:
    Window(const std::string& appName, int width, int height);
    ~Window();

    glm::ivec2 GetFramebufferSize();

    void RegisterInput(const InputSystem& inputSystem);
    void UnregisterInput(const InputSystem& inputSystem);

    bool ShouldClose();

    GLFWwindow* GetRawWindow();

private:
    GLFWwindow* _glfwWindow;
    int _width, _height;
};

}
