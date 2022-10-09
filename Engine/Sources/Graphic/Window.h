#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"

#include <glm/glm.hpp>

#include <string>


struct GLFWwindow;

namespace ZE {

class InputSystem;
class VulkanDevice;
class VulkanSurface;
class VulkanSwapchain;

class Window
{
public:
    Window(const std::string& title, const glm::ivec2& size);
    ~Window();

    void CreateSurfaceAndSwapchain(TPtr<VulkanDevice> device);

    bool ShouldClose();

    glm::ivec2 GetFramebufferSize();

    TPtr<VulkanSurface> GetSurface();
    TPtr<VulkanSwapchain> GetSwapchain();

    GLFWwindow* GetRawWindow();

    // input
    void RegisterInput(const InputSystem& inputSystem);
    void UnregisterInput(const InputSystem& inputSystem);

private:
    GLFWwindow* _glfwWindow;
    glm::ivec2 _size;

    TPtr<VulkanSurface> _surface;
    TPtr<VulkanSwapchain> _swapchain;
};

} // namespace ZE
