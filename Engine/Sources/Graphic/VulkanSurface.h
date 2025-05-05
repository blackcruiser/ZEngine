#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"

#include <vulkan/vulkan.h>


namespace ZE {

class VulkanSurface
{
public:
    VulkanSurface(VkInstance instance, void* window);
    ~VulkanSurface();

    void InitializeFormat(VkPhysicalDevice physicalDevice, VkSurfaceFormatKHR formatHint = {VkFormat::VK_FORMAT_B8G8R8A8_SRGB, VkColorSpaceKHR::VK_COLOR_SPACE_SRGB_NONLINEAR_KHR});
    void InitializePresentMode(VkPhysicalDevice physicalDevice, VkPresentModeKHR presentModeHint = VkPresentModeKHR::VK_PRESENT_MODE_MAILBOX_KHR);
    void InitializeExtent(VkPhysicalDevice physicalDevice, const VkExtent2D& extentHint);

    VkSurfaceKHR GetRawSurface();
    VkSurfaceFormatKHR GetSurfaceFormat();
    VkPresentModeKHR GetPresentMode();
    VkExtent2D GetExtent();

    std::vector<VkSurfaceFormatKHR> GetSupportedSurfaceFormats(VkPhysicalDevice physicalDevice);
    std::vector<VkPresentModeKHR> GetSupportedPresentModes(VkPhysicalDevice physicalDevice);
    VkSurfaceCapabilitiesKHR GetCpabilities(VkPhysicalDevice physicalDevice);

private:
    VkInstance _instance;
    VkSurfaceKHR _surface;
    VkSurfaceFormatKHR _surfaceFormat;
    VkPresentModeKHR _presentMode;
    VkExtent2D _extent;
};

} // namespace ZE 