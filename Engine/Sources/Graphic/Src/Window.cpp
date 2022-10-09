#include "Window.h"
#include "Input/InputSystem.h"
#include "Graphic/VulkanGPU.h"
#include "Graphic/VulkanDevice.h"
#include "Graphic/VulkanSurface.h"
#include "Graphic/VulkanSwapchain.h"
#include "Graphic/VulkanSurface.h"
#include "Graphic/VulkanSurface.h"
#include "Graphic/VulkanDevice.h"
#include "Graphic/VulkanDevice.h"

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
}

Window::~Window()
{
    _swapchain.reset();
    _surface.reset();

    glfwDestroyWindow(_glfwWindow);
    glfwTerminate();
}

void Window::CreateSurfaceAndSwapchain(TPtr<VulkanDevice> device)
{
    TPtr<VulkanGPU> GPU = device->GetGPU();

    _surface = std::make_shared<VulkanSurface>(GPU->GetVkInstance(), _glfwWindow);

    VkExtent2D extent{_size.x, _size.y};
    _surface->InitializeExtent(GPU, extent);
    VkSurfaceFormatKHR surfaceFormat{ VkFormat::VK_FORMAT_B8G8R8A8_UNORM, VkColorSpaceKHR::VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
    _surface->InitializeFormat(GPU, surfaceFormat);
    _surface->InitializePresentMode(GPU, VkPresentModeKHR::VK_PRESENT_MODE_MAILBOX_KHR);

    _swapchain = std::make_shared<VulkanSwapchain>(device, _surface, 3);
}

bool Window::ShouldClose()
{
    return glfwWindowShouldClose(_glfwWindow);
}

glm::ivec2 Window::GetFramebufferSize()
{
    glm::ivec2 size;
    glfwGetFramebufferSize(_glfwWindow, &size.x, &size.y);

    return size;
}

TPtr<VulkanSurface> Window::GetSurface()
{
    return _surface;
}

TPtr<VulkanSwapchain> Window::GetSwapchain()
{
    return _swapchain;
}


GLFWwindow* Window::GetRawWindow()
{
    return _glfwWindow;
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

} // namespace ZE