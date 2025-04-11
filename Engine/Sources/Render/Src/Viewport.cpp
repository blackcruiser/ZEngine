#include "Viewport.h"
#include "Graphic/VulkanDevice.h"
#include "Graphic/VulkanSwapchain.h"
#include "Graphic/VulkanQueue.h"
#include "Render/RenderSystem.h"
#include "Render/RenderingContext.h"
#include "Render/RenderGraph.h"


namespace ZE {

Viewport::Viewport(const glm::ivec2& size, TPtr<VulkanSwapchain> swapchain) :
    _size(size), _swapchain(swapchain)
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
    TPtr<VulkanDevice> device = RenderSystem::Get().GetDevice();
    VkSemaphore submitSemaphore = device->CreateGraphicSemaphore();
    VkFence fence = device->CreateFence(false);

    TPtr<VulkanQueue> graphicQueue = renderingContext->GetQueue(VulkanQueue::EType::Graphic);
    graphicQueue->Submit(renderGraph->GetCommandBuffer(), {}, {}, {submitSemaphore}, fence);
    graphicQueue->Present(_swapchain, {submitSemaphore});

    vkWaitForFences(device->GetRawDevice(), 1, &fence, VK_TRUE, UINT64_MAX);

    device->DestroyFence(fence);
    device->DestroyGraphicSemaphore(submitSemaphore);
}

}