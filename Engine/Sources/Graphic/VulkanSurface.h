#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"

#include <vulkan/vulkan.h>

struct GLFWwindow;

namespace ZE {

class VulkanGPU;

class VulkanSurface
{
public:
    VulkanSurface(VkInstance vkInstance, GLFWwindow* window);
    ~VulkanSurface();

    void InitializeFormat(TPtr<VulkanGPU> GPU, VkSurfaceFormatKHR formatHint = {VkFormat::VK_FORMAT_B8G8R8A8_SRGB, VkColorSpaceKHR::VK_COLOR_SPACE_SRGB_NONLINEAR_KHR});
    void InitializePresentMode(TPtr<VulkanGPU> GPU, VkPresentModeKHR presentModeHint = VkPresentModeKHR::VK_PRESENT_MODE_MAILBOX_KHR);
    void InitializeExtent(TPtr<VulkanGPU> GPU, const VkExtent2D& extentHint);

    VkSurfaceKHR GetRawSurface();
    VkSurfaceFormatKHR GetSurfaceFormat();
    VkPresentModeKHR GetPresentMode();
    VkExtent2D GetExtent();

    std::vector<VkSurfaceFormatKHR> GetSupportedSurfaceFormats(TPtr<VulkanGPU> GPU);
    std::vector<VkPresentModeKHR> GetSupportedPresentModes(TPtr<VulkanGPU> GPU);
    VkSurfaceCapabilitiesKHR GetCpabilities(TPtr<VulkanGPU> GPU);

private:
    VkInstance _vkInstance;
    VkSurfaceKHR _vkSurface;
    VkSurfaceFormatKHR _vkSurfaceFormat;
    VkPresentModeKHR _presentMode;
    VkExtent2D _vkExtent;
};

} // namespace ZE