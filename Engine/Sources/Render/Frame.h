#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"

#include <vulkan/vulkan.h>

namespace ZE {

struct RenderTargets;
class VulkanSwapchain;
class VulkanFramebuffer;
class VulkanImageView;
class VulkanDevice;
class VulkanCommandBuffer;

class Frame
{
public:
    Frame(TPtr<VulkanDevice> device, TPtr<VulkanSwapchain> swapchain);
    ~Frame();

    TPtr<RenderTargets> GetFrameBuffer();
    VkExtent3D GetExtent();

    VkSemaphore GetAvailableSemaphore();
    VkFence GetFence();

    TPtr<VulkanCommandBuffer> GetCachedCommandBuffer();

    void PutImage(TPtr<VulkanImageView> imageView); 
    void PutFramebuffer(TPtr<VulkanFramebuffer> framebuffer);
    void PutSemaphore(VkSemaphore sem);

private:
    TPtr<VulkanDevice> _cachedDevice;
    TPtr<RenderTargets> _renderTargets;

    VkExtent3D _extent;
    VkSemaphore _imageAvailableSemaphore;
    VkFence _fence;

    TPtrArr<VulkanImageView> _imageViewArr;
    TPtrArr<VulkanFramebuffer> _framebufferArr;
    std::vector<VkSemaphore> _semaphoreArr;
    TPtr<VulkanCommandBuffer> _cachedCommandBuffer;
};

}