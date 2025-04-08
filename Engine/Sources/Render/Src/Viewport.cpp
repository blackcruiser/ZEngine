#include "Viewport.h"
#include "Graphic/VulkanSwapchain.h"
#include "Graphic/VulkanQueue.h"


namespace ZE {

Viewport::Viewport(const glm::ivec2& size, TPtr<VulkanSwapchain> swapchain)
{

}

glm::ivec2 Viewport::GetSize()
{
    return _size;
}

TPtr<VulkanImage> Viewport::GetRenderTarget()
{
    return _swapchain->AcquireNextImage(UINT64_MAX, _semaphore, _fence);
}

void Viewport::Present(TPtr<RenderingContext> renderingContext, TPtr<RenderGraph> renderGraph)
{
    VkSemaphore submitSemaphore = _device->CreateGraphicSemaphore();

    TPtr<VulkanQueue> graphicQueue = GetQueue(VulkanQueue::EType::Graphic);
    graphicQueue->Submit(renderGraph->GetCommandBuffer(), {}, {}, {submitSemaphore}, renderGraph->GetFence());
    graphicQueue->Present(_swapchain, {submitSemaphore});

    VkFence fence = renderGraph->GetFence();
    vkWaitForFences(_device->GetRawDevice(), 1, &fence, VK_TRUE, UINT64_MAX);
}

}