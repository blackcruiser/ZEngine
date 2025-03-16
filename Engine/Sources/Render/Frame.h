#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"

#include <glm/glm.hpp>
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

    const glm::ivec2 GetViewport();

    TPtr<VulkanImageView> GetFrameBuffer();
    VkExtent3D GetExtent();

    VkSemaphore GetAvailableSemaphore();
    VkFence GetFence();

    TPtr<VulkanCommandBuffer> GetCachedCommandBuffer();

    void PutImage(TPtr<VulkanImageView> imageView); 
    void PutFramebuffer(TPtr<VulkanFramebuffer> framebuffer);
    void PutSemaphore(VkSemaphore sem);

private:
    TPtr<VulkanDevice> _cachedDevice;
    TPtr<VulkanImageView> _renderTarget;

    VkExtent3D _extent;
    VkSemaphore _imageAvailableSemaphore;
    VkFence _fence;

    TPtrArr<VulkanImageView> _imageViewArr;
    TPtrArr<VulkanFramebuffer> _framebufferArr;
    std::vector<VkSemaphore> _semaphoreArr;
    TPtr<VulkanCommandBuffer> _cachedCommandBuffer;
};

}