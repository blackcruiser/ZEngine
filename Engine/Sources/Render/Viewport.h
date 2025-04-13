#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"
#include "RenderTargets.h"

#include <glm/vec2.hpp>
#include <vulkan/vulkan_core.h>


namespace ZE {

class VulkanImage;
class VulkanSwapchain;
class RenderingContext;
class RenderGraph;

class Viewport
{
public:
    Viewport(const glm::ivec2& size, TPtr<VulkanSwapchain> swapchain);
    ~Viewport();

    glm::ivec2 GetSize();
    
    TPtr<VulkanImage> GetRenderTarget();

    void Present(TPtr<RenderingContext> renderingContext, TPtr<RenderGraph> renderGraph);

private:
    TPtr<VulkanSwapchain> _swapchain;
    VkSemaphore _submitSemaphore, _presentSemaphore;
    TPtr<VulkanImage> _currentImage;
    glm::ivec2 _size;
};

}