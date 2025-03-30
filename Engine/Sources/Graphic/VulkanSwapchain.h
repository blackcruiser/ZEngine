#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"

#include <vulkan/vulkan.h>
#include <glm/vec2.hpp>


namespace ZE {

class VulkanSurface;
class VulkanDevice;
class VulkanImage;

class VulkanSwapchain
{
public:
    VulkanSwapchain(TPtr<VulkanDevice> device, void* windowHandle, const glm::ivec2& size, uint32_t imageCount);
    ~VulkanSwapchain();

    uint32_t GetImageCount();
    uint32_t GetCurrentIndex();
    TPtr<VulkanImage> GetCurrentImage();
    bool AcquireNextImage(uint64_t timeout, VkSemaphore semaphore, VkFence fence);

    TPtr<VulkanDevice> GetDevice();
    VkSwapchainKHR GetRawSwapchain();

private:
    VkSwapchainKHR _vkSwapchain;
    uint32_t _imageCount;
    uint32_t _acquiredIndex;

    TPtr<VulkanDevice> _device;
    TPtr<VulkanSurface> _surface;
    TPtrArr<VulkanImage> _imagerArr;
};

} // namespace ZE