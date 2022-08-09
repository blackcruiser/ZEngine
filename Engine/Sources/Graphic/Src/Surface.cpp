#include "Surface.h"
#include "GPU.h"
#include "Window.h"

#include <glfw/glfw3.h>

#include <stdexcept>
#include <algorithm>

namespace TE {

Surface::Surface(VkInstance vkInstance, TPtr<GPU> GPU, TPtr<Window> window) : _vkInstance(vkInstance), _GPU(GPU), _window(window)
{
    if (glfwCreateWindowSurface(_vkInstance, _window->GetRawWindow(), nullptr, &_vkSurface) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create window surface!");
    }

    _SelectSurfaceFormat();
    _SelectPresentMode();
    _CalculateExtent();
}

Surface::~Surface()
{
    vkDestroySurfaceKHR(_vkInstance, _vkSurface, nullptr);
}

VkSurfaceKHR Surface::GetRawSurface()
{
    return _vkSurface;
}

VkSurfaceFormatKHR Surface::GetSurfaceFormat()
{
    return _vkSurfaceFormat;
}

VkPresentModeKHR Surface::GetPresentMode()
{
    return _presentMode;
}

VkExtent2D Surface::GetExtent()
{
    return _vkExtent;
}

std::vector<VkSurfaceFormatKHR> Surface::GetSupportedSurfaceFormats()
{
    VkPhysicalDevice physicalDevice = _GPU->GetRawPhysicalDevice();

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, _vkSurface, &formatCount, nullptr);

    std::vector<VkSurfaceFormatKHR> surfaceFormats;
    if (formatCount != 0)
    {
        surfaceFormats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, _vkSurface, &formatCount, surfaceFormats.data());
    }

    return surfaceFormats;
}

std::vector<VkPresentModeKHR> Surface::GetSupportedPresentModes()
{
    VkPhysicalDevice physicalDevice = _GPU->GetRawPhysicalDevice();

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, _vkSurface, &presentModeCount, nullptr);

    std::vector<VkPresentModeKHR> presentModes;
    if (presentModeCount != 0)
    {
        presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, _vkSurface, &presentModeCount, presentModes.data());
    }

    return presentModes;
}

VkSurfaceCapabilitiesKHR Surface::GetCpabilities()
{
    VkPhysicalDevice physicalDevice = _GPU->GetRawPhysicalDevice();
    VkSurfaceCapabilitiesKHR capabilities{};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, _vkSurface, &capabilities);

    return capabilities;
}

void Surface::_SelectSurfaceFormat()
{
    std::vector<VkSurfaceFormatKHR> surfaceFormats = GetSupportedSurfaceFormats();

    for (size_t i = 0; i < surfaceFormats.size(); i++)
    {
        const VkSurfaceFormatKHR& currentSurfaceFormat = surfaceFormats[i];

        if (currentSurfaceFormat.colorSpace == VkColorSpaceKHR::VK_COLOR_SPACE_SRGB_NONLINEAR_KHR && currentSurfaceFormat.format == VkFormat::VK_FORMAT_B8G8R8A8_SRGB)
        {
            _vkSurfaceFormat = currentSurfaceFormat;
            break;
        }
    }
}

void Surface::_SelectPresentMode()
{
    std::vector<VkPresentModeKHR> supportedPresentModes = GetSupportedPresentModes();

    VkPresentModeKHR presentMode{ VkPresentModeKHR::VK_PRESENT_MODE_IMMEDIATE_KHR };
    for (const VkPresentModeKHR& presentMode : supportedPresentModes)
    {
        if (presentMode == VkPresentModeKHR::VK_PRESENT_MODE_MAILBOX_KHR)
            break;
    }
    _presentMode = presentMode;
}

void Surface::_CalculateExtent()
{
    VkSurfaceCapabilitiesKHR capabilities = GetCpabilities();

    // GLFW uses two units when measuring sizes: pixels and screen coordinates.
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        _vkExtent = capabilities.currentExtent;
    }
    else
    {
        // glfwGetFramebufferSize: resolution of the window in pixel
        glm::ivec2 size = _window->GetFramebufferSize();

        _vkExtent.width = std::clamp(static_cast<uint32_t>(size.x), capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        _vkExtent.height = std::clamp(static_cast<uint32_t>(size.y), capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    }
}

}