#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"
#include "RenderTargets.h"

#include <glm/vec2.hpp>
#include <vulkan/vulkan_core.h>


namespace ZE {

class VulkanImage;
class VulkanSwapchain;

class Viewport
{
public:
    Viewport(const glm::ivec2& size);

    glm::ivec2 GetSize();
    
    TPtr<VulkanImage> GetRenderTarget();

private:
    TPtr<VulkanSwapchain> _swapchain;
    VkSemaphore _semaphore;
    VkFence _fence;
    glm::ivec2 _size;
};

}