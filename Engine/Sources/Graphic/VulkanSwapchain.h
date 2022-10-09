#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"

#include <vulkan/vulkan.h>


namespace ZE {

class VulkanSurface;
class VulkanDevice;
class VulkanImage;

class VulkanSwapchain
{
public:
    VulkanSwapchain(TPtr<VulkanDevice> device, TPtr<VulkanSurface> surface, uint32_t imageCount);
    ~VulkanSwapchain();

    uint32_t GetCurrentAcquiredIndex();
    TPtr<VulkanImage> AcquireNextImage(uint64_t timeout, VkSemaphore semaphore, VkFence fence);

    VkSwapchainKHR GetRawSwapchain();

private:
    VkSwapchainKHR _vkSwapchain;
    uint32_t _acquiredIndex;

    TPtr<VulkanDevice> _device;
    TPtr<VulkanSurface> _surface;
    TPtrArr<VulkanImage> _imagerArr;
};

} // namespace ZE