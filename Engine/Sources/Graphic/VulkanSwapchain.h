#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"
#include "VulkanDevice.h"

#include <glm/vec2.hpp>


namespace ZE {

class VulkanSurface;
class VulkanDevice;
class VulkanImage;

class VulkanSwapchain : public VulkanDeviceChild
{
public:
    VulkanSwapchain(VulkanDevice* device, void* windowHandle, const glm::ivec2& size, uint32_t imageCount);
    ~VulkanSwapchain();

    uint32_t GetImageCount();
    uint32_t GetCurrentIndex();
    VulkanImage* GetCurrentImage();
    bool AcquireNextImage(uint64_t timeout, VkSemaphore semaphore, VkFence fence);

    VkSwapchainKHR GetRawSwapchain();

private:
    VkSwapchainKHR _swapchain;
    uint32_t _imageCount;
    uint32_t _acquiredIndex;

    VulkanSurface* _surface;
    std::vector<VulkanImage*> _imagerArr;
};

} // namespace ZE