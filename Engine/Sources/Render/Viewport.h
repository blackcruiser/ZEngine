#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"
#include "RenderTargets.h"
#include "RenderResource.h"

#include <glm/vec2.hpp>
#include <vulkan/vulkan_core.h>


namespace ZE {

class VulkanDevice;
class VulkanImage;
class VulkanSwapchain;
class RenderGraph;

class Viewport : public RenderResource
{
public:
    Viewport(void* windowHandle, const glm::ivec2& size);
    ~Viewport();

    virtual void InitRenderResource(TPtr<RenderGraph> renderGraph) override;
    virtual void CleanupRenderResource(TPtr<RenderGraph> renderGraph) override;

    glm::ivec2 GetSize();
    
    VulkanImage* GetCurrentImage();

    void Advance();

    void Present(TPtr<RenderGraph> renderGraph);

private:
    void* _windowHandle;
    VulkanSwapchain* _swapchain;
    std::vector<VkSemaphore> _submitSemaphores, _presentSemaphores;
    std::vector<VkFence> _presentFences;
    uint32_t _currentIndex;
    glm::ivec2 _size;
};

}