#include "Viewport.h"
#include "Graphic/VulkanDevice.h"
#include "Graphic/VulkanSwapchain.h"
#include "Graphic/VulkanQueue.h"
#include "Graphic/VulkanSynchronizer.h"
#include "Render/RenderSystem.h"
#include "Render/RenderGraph.h"
#include "CoreTypes.h"
#include <iostream>


const uint32_t kImageCount = 3;

namespace ZE {

Viewport::Viewport(void* windowHandle, const glm::ivec2& size) : _size(size), _currentIndex(0), _windowHandle(windowHandle), _queuedImageCount(0)
{
}

Viewport::~Viewport()
{
}

void Viewport::InitRenderResource(RenderGraph* renderGraph)
{
    RenderResource::InitRenderResource(renderGraph);

    VulkanDevice* device = renderGraph->GetDevice();
    _swapchain = new VulkanSwapchain(device, _windowHandle, _size, kImageCount);

    for (uint32_t i = 0; i < kImageCount; i++)
    {
        _submitSemaphores.emplace_back(CreateSemaphore(device));
        _presentSemaphores.emplace_back(CreateSemaphore(device));
        _presentFences.emplace_back(CreateFence(device, true));
    }
}

void Viewport::CleanupRenderResource(RenderGraph* renderGraph)
{
    VulkanDevice* device = renderGraph->GetDevice();
    uint32_t imageCount = _swapchain->GetImageCount();
    for (uint32_t i = 0; i < imageCount; i++)
    {
        DestroySemaphore(device, _submitSemaphores[i]);
        DestroySemaphore(device, _presentSemaphores[i]);
        DestroyFence(device, _presentFences[i]);
    }

    delete _swapchain;

    RenderResource::CleanupRenderResource(renderGraph);
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
   // VkDevice device = _swapchain->GetDevice()->GetRawDevice();
    //VkFence fence = _presentFences[0];
    //vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);
    //vkResetFences(device, 1, &fence);
    
    //std::cout << "AcquireNextImage" << std::endl;
    _queuedImageCount++;
    VkFence fence = VK_NULL_HANDLE;
    if (_queuedImageCount >= _swapchain->GetImageCount())
        fence = _presentFences[0];

     _swapchain->AcquireNextImage(UINT64_MAX, _submitSemaphores[_currentIndex], fence);

     if (fence != VK_NULL_HANDLE)
     {
         VkDevice device = _swapchain->GetDevice()->GetRawDevice();
         vkWaitForFences(device, 1, &fence, true, UINT64_MAX);
         vkResetFences(device, 1, &fence);
     }
}

void Viewport::Present(RenderGraph* renderGraph)
{
    _swapchain->AcquireNextImage(UINT64_MAX, _submitSemaphores[_currentIndex], VK_NULL_HANDLE);

    TPtr<VulkanImage> currentImage = GetCurrentImage();
    renderGraph->TransitionLayout(currentImage, VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VkImageLayout::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    //std::cout << "Execute" << std::endl;
    renderGraph->Execute({_submitSemaphores[_currentIndex]}, {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT}, {_presentSemaphores[_currentIndex]});

    //std::cout << "Present" << std::endl;
    renderGraph->Present(_swapchain, {_presentSemaphores[_currentIndex]});
    _queuedImageCount--;
}

}