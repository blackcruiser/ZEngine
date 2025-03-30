#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"
#include "Viewport.h"

#include <glm/glm.hpp>

#include <string>


struct GLFWwindow;

namespace ZE {

class InputSystem;
class VulkanDevice;
class Viewport;

class Window
{
public:
    Window(const std::string& title, const glm::ivec2& size);
    ~Window();

    bool ShouldClose();

    void CreateViewport(TPtr<VulkanDevice> device);
    TPtr<Viewport> GetViewport();

    glm::ivec2 GetSize();

    // input
    void RegisterInput(const InputSystem& inputSystem);
    void UnregisterInput(const InputSystem& inputSystem);

private:
    GLFWwindow* _glfwWindow;
    glm::ivec2 _size;
    TPtr<Viewport> _viewport;
};

} // namespace ZE
