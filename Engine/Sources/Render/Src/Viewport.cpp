#include "Viewport.h"
#include "Graphic/VulkanDevice.h"
#include "Graphic/VulkanSwapchain.h"
#include "Graphic/VulkanQueue.h"
#include "Render/RenderSystem.h"
#include "Render/RenderGraph.h"


const uint32_t kImageCount = 3;

namespace ZE {

Viewport::Viewport(TPtr<VulkanDevice> device, void* windowHandle, const glm::ivec2& size) :
    _size(size), _currentIndex(0)
{
    _swapchain = std::make_shared<VulkanSwapchain>(device, windowHandle, size, kImageCount);

    for (uint32_t i = 0; i < kImageCount; i++)
    {
        _submitSemaphores.emplace_back(device->CreateGraphicSemaphore());
        _presentSemaphores.emplace_back(device->CreateGraphicSemaphore());
        _presentFences.emplace_back(device->CreateFence(true));
    }
}

Viewport::~Viewport()
{
    TPtr<VulkanDevice> device = RenderSystem::Get().GetDevice();

    uint32_t imageCount = _swapchain->GetImageCount();
    for (uint32_t i = 0; i < imageCount; i++)
    {
        device->DestroyGraphicSemaphore(_submitSemaphores[i]);
        device->DestroyGraphicSemaphore(_presentSemaphores[i]);
        device->DestroyFence(_presentFences[i]);
    }
}

glm::ivec2 Viewport::GetSize()
{
    return _size;
}

TPtr<VulkanImage> Viewport::GetCurrentImage()
{
    return _swapchain->GetCurrentImage();
}

void Viewport::Advance()
{
    _currentIndex = (_currentIndex + 1) % _swapchain->GetImageCount();
    VkDevice device = _swapchain->GetDevice()->GetRawDevice();
    VkFence fence = _presentFences[_currentIndex];
    vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);
    vkResetFences(device, 1, &fence);

     _swapchain->AcquireNextImage(UINT64_MAX, _submitSemaphores[_currentIndex], _presentFences[_currentIndex]);
}

void Viewport::Present(TPtr<RenderGraph> renderGraph)
{
    TPtr<VulkanImage> currentImage = GetCurrentImage();
    renderGraph->TransitionLayout(currentImage, VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VkImageLayout::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    renderGraph->Execute({_submitSemaphores[_currentIndex]}, {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT}, {_presentSemaphores[_currentIndex]});

    TPtr<VulkanQueue> graphicQueue = RenderSystem::Get().GetQueue(VulkanQueue::EType::Graphic);
    graphicQueue->Present(_swapchain, {_presentSemaphores[_currentIndex]});
}

}