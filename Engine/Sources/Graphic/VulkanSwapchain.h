#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"
#include "VulkanDevice.h"
#include "GraphicResource.h"

#include <glm/vec2.hpp>


namespace ZE {

class VulkanSurface;
class VulkanDevice;
class VulkanImage;

class VulkanSwapchain : public GraphicResource, public VulkanDeviceChild
{
public:
    VulkanSwapchain(VulkanDevice* device, void* windowHandle, const glm::ivec2& size, uint32_t imageCount);
    ~VulkanSwapchain();

    uint32_t GetImageCount();
    uint32_t GetCurrentIndex();
    TPtr<VulkanImage> GetCurrentImage();
    bool AcquireNextImage(uint64_t timeout, VkSemaphore semaphore, VkFence fence);

    VkSwapchainKHR GetRawSwapchain();

private:
    VkSwapchainKHR _swapchain;
    uint32_t _imageCount;
    uint32_t _acquiredIndex;

    VulkanSurface* _surface;
    std::vector<TPtr<VulkanImage>> _imagerArr;
};

} // namespace ZE