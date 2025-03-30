#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"
#include "RenderTargets.h"

#include <glm/vec2.hpp>
#include <vulkan/vulkan_core.h>


namespace ZE {

class VulkanDevice;
class VulkanImage;
class VulkanSwapchain;
class RenderGraph;

class Viewport
{
public:
    Viewport(TPtr<VulkanDevice> device, void* windowHandle, const glm::ivec2& size);
    ~Viewport();

    glm::ivec2 GetSize();
    
    TPtr<VulkanImage> GetCurrentImage();

    void Advance();

    void Present(TPtr<RenderGraph> renderGraph);

private:
    TPtr<VulkanSwapchain> _swapchain;
    std::vector<VkSemaphore> _submitSemaphores, _presentSemaphores;
    std::vector<VkFence> _presentFences;
    uint32_t _currentIndex;
    glm::ivec2 _size;
};

}