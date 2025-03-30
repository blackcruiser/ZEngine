#include "VulkanSurface.h"
#include "VulkanGPU.h"
#include "Window.h"

#include <glfw/glfw3.h>

#include <stdexcept>
#include <algorithm>

namespace ZE {

VulkanSurface::VulkanSurface(VkInstance vkInstance, void* window)
    : _vkInstance(vkInstance), _vkSurface(VK_NULL_HANDLE)
{
    if (glfwCreateWindowSurface(_vkInstance, (GLFWwindow*)window, nullptr, &_vkSurface) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create window surface!");
    }
}

VulkanSurface::~VulkanSurface()
{
    if (_vkSurface != VK_NULL_HANDLE)
        vkDestroySurfaceKHR(_vkInstance, _vkSurface, nullptr);
}

void VulkanSurface::InitializeFormat(TPtr<VulkanGPU> GPU, VkSurfaceFormatKHR formatHint)
{
    std::vector<VkSurfaceFormatKHR> surfaceFormats = GetSupportedSurfaceFormats(GPU);

    for (const VkSurfaceFormatKHR& currentSurfaceFormat : surfaceFormats)
    {
        if (currentSurfaceFormat.colorSpace == formatHint.colorSpace && currentSurfaceFormat.format == formatHint.format)
        {
            _vkSurfaceFormat = currentSurfaceFormat;
            return;
        }
    }

    _vkSurfaceFormat = surfaceFormats[0];
}

void VulkanSurface::InitializePresentMode(TPtr<VulkanGPU> GPU, VkPresentModeKHR presentModeHint)
{
    std::vector<VkPresentModeKHR> supportedPresentModes = GetSupportedPresentModes(GPU);

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

void VulkanSurface::InitializeExtent(TPtr<VulkanGPU> GPU, const VkExtent2D& extentHint)
{
    VkSurfaceCapabilitiesKHR capabilities = GetCpabilities(GPU);

    // GLFW uses two units when measuring sizes: pixels and screen coordinates.
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        _vkExtent = capabilities.currentExtent;
    }
    else
    {
        _vkExtent.width = std::clamp(extentHint.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        _vkExtent.height = std::clamp(extentHint.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    }
}

VkSurfaceKHR VulkanSurface::GetRawSurface()
{
    return _vkSurface;
}

VkSurfaceFormatKHR VulkanSurface::GetSurfaceFormat()
{
    return _vkSurfaceFormat;
}

VkPresentModeKHR VulkanSurface::GetPresentMode()
{
    return _presentMode;
}

VkExtent2D VulkanSurface::GetExtent()
{
    return _vkExtent;
}

std::vector<VkSurfaceFormatKHR> VulkanSurface::GetSupportedSurfaceFormats(TPtr<VulkanGPU> GPU)
{
    VkPhysicalDevice physicalDevice = GPU->GetRawGPU();

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

std::vector<VkPresentModeKHR> VulkanSurface::GetSupportedPresentModes(TPtr<VulkanGPU> GPU)
{
    VkPhysicalDevice physicalDevice = GPU->GetRawGPU();

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

VkSurfaceCapabilitiesKHR VulkanSurface::GetCpabilities(TPtr<VulkanGPU> GPU)
{
    VkPhysicalDevice physicalDevice = GPU->GetRawGPU();
    VkSurfaceCapabilitiesKHR capabilities{};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, _vkSurface, &capabilities);

    return capabilities;
}

} // namespace ZE