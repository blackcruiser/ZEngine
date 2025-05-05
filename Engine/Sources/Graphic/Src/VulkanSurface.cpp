#include "VulkanSurface.h"
#include "VulkanContext.h"
#include "Misc/AssertionMacros.h"

#include <glfw/glfw3.h>

#include <algorithm>


namespace ZE {

VulkanSurface::VulkanSurface(VkInstance instance, void* window)
    : _instance(instance), _surface(VK_NULL_HANDLE)
{
    VkResult result = glfwCreateWindowSurface(_instance, (GLFWwindow*)window, nullptr, &_surface);
    CHECK_MSG(result == VkResult::VK_SUCCESS, "Failed to create surface!");
}

VulkanSurface::~VulkanSurface()
{
    CHECK(_surface != VK_NULL_HANDLE);
    vkDestroySurfaceKHR(_instance, _surface, nullptr);
}

void VulkanSurface::InitializeFormat(VkPhysicalDevice physicalDevice, VkSurfaceFormatKHR formatHint)
{
    std::vector<VkSurfaceFormatKHR> surfaceFormats = GetSupportedSurfaceFormats(physicalDevice);

    for (const VkSurfaceFormatKHR& currentSurfaceFormat : surfaceFormats)
    {
        if (currentSurfaceFormat.colorSpace == formatHint.colorSpace && currentSurfaceFormat.format == formatHint.format)
        {
            _surfaceFormat = currentSurfaceFormat;
            return;
        }
    }

    _surfaceFormat = surfaceFormats[0];
}

void VulkanSurface::InitializePresentMode(VkPhysicalDevice physicalDevice, VkPresentModeKHR presentModeHint)
{
    std::vector<VkPresentModeKHR> supportedPresentModes = GetSupportedPresentModes(physicalDevice);

    _presentMode = VkPresentModeKHR::VK_PRESENT_MODE_IMMEDIATE_KHR;
    for (const VkPresentModeKHR& presentMode : supportedPresentModes)
    {
        if (presentMode == presentModeHint)
        {
            _presentMode = presentMode;
            break;
        }
    }
}

void VulkanSurface::InitializeExtent(VkPhysicalDevice physicalDevice, const VkExtent2D& extentHint)
{
    VkSurfaceCapabilitiesKHR capabilities = GetCpabilities(physicalDevice);

    // GLFW uses two units when measuring sizes: pixels and screen coordinates.
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        _extent = capabilities.currentExtent;
    }
    else
    {
        _extent.width = std::clamp(extentHint.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        _extent.height = std::clamp(extentHint.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    }
}

VkSurfaceKHR VulkanSurface::GetRawSurface()
{
    return _surface;
}

VkSurfaceFormatKHR VulkanSurface::GetSurfaceFormat()
{
    return _surfaceFormat;
}

VkPresentModeKHR VulkanSurface::GetPresentMode()
{
    return _presentMode;
}

VkExtent2D VulkanSurface::GetExtent()
{
    return _extent;
}

std::vector<VkSurfaceFormatKHR> VulkanSurface::GetSupportedSurfaceFormats(VkPhysicalDevice physicalDevice)
{
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, _surface, &formatCount, nullptr);

    std::vector<VkSurfaceFormatKHR> surfaceFormats;
    if (formatCount != 0)
    {
        surfaceFormats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, _surface, &formatCount, surfaceFormats.data());
    }

    return surfaceFormats;
}

std::vector<VkPresentModeKHR> VulkanSurface::GetSupportedPresentModes(VkPhysicalDevice physicalDevice)
{
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, _surface, &presentModeCount, nullptr);

    std::vector<VkPresentModeKHR> presentModes;
    if (presentModeCount != 0)
    {
        presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, _surface, &presentModeCount, presentModes.data());
    }

    return presentModes;
}

VkSurfaceCapabilitiesKHR VulkanSurface::GetCpabilities(VkPhysicalDevice physicalDevice)
{
    VkSurfaceCapabilitiesKHR capabilities{};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, _surface, &capabilities);

    return capabilities;
}

} // namespace ZE