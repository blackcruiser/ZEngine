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
    TPtr<VulkanDevice> device = RenderSystem::Get().GetDevice();
    _submitSemaphore = device->CreateGraphicSemaphore();
    _presentSemaphore = device->CreateGraphicSemaphore();
}

Viewport::~Viewport()
{
    TPtr<VulkanDevice> device = RenderSystem::Get().GetDevice();
    device->DestroyGraphicSemaphore(_submitSemaphore);
    device->DestroyGraphicSemaphore(_presentSemaphore);
}

glm::ivec2 Viewport::GetSize()
{
    return _size;
}

TPtr<VulkanImage> Viewport::GetRenderTarget()
{
    _currentImage =  _swapchain->AcquireNextImage(UINT64_MAX, _submitSemaphore, VK_NULL_HANDLE);
    return _currentImage;
}

void Viewport::Present(TPtr<RenderingContext> renderingContext, TPtr<RenderGraph> renderGraph)
{
    TPtr<VulkanDevice> device = RenderSystem::Get().GetDevice();
    VkFence fence = device->CreateFence(false);

    renderGraph->TransitionLayout(_currentImage, VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VkImageLayout::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    renderGraph->Execute({_submitSemaphore}, {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT}, {_presentSemaphore}, fence);

    TPtr<VulkanQueue> graphicQueue = renderingContext->GetQueue(VulkanQueue::EType::Graphic);
    graphicQueue->Present(_swapchain, {_presentSemaphore});

    //vkWaitForFences(device->GetRawDevice(), 1, &fence, VK_TRUE, UINT64_MAX);

    device->DestroyFence(fence);
}

}