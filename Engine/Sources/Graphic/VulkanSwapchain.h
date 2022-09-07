#pragma once

#include "CoreDefines.h"

#include <vulkan/vulkan.hpp>

namespace TE {

class Surface;
class VulkanDevice;
class VulkanImage;

class VulkanSwapchain
{
public:
    VulkanSwapchain(TPtr<VulkanDevice> device, TPtr<Surface> surface, uint32_t imageCount);
    ~VulkanSwapchain();

    TPtrArr<VulkanImage> GetImages();
    uint32_t AcquireNextImage(VkSemaphore semaphore);

    VkSwapchainKHR GetRawSwapchain();

private:
    VkExtent2D _extent;
    VkFormat _format;

    TPtr<VulkanDevice> _device;
    TPtr<Surface> _surface;

    VkSwapchainKHR _vkSwapchain;
};

} // namespace TE