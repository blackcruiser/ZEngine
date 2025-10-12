#include "Viewport.h"
#include "Graphic/VulkanDevice.h"
#include "Graphic/VulkanSwapchain.h"
#include "Graphic/VulkanQueue.h"
#include "Graphic/VulkanSynchronizer.h"
#include "Render/RenderSystem.h"
#include "Render/RenderGraph.h"
#include "Render/RenderSynchronizer.h"
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

void Viewport::InitGraphic(RenderGraph* renderGraph)
{
    RenderResource::InitGraphic(renderGraph);

    VulkanDevice* device = renderGraph->GetDevice();
    _swapchain = new VulkanSwapchain(device, _windowHandle, _size, kImageCount);
}

void Viewport::CleanupGraphic()
{
    uint32_t imageCount = _swapchain->GetImageCount();
    delete _swapchain;

    RenderResource::CleanupGraphic();
}

glm::ivec2 Viewport::GetSize()
{
    return _size;
}

TPtr<VulkanImage> Viewport::GetCurrentImage()
{
    return _swapchain->GetCurrentImage();
}

void Viewport::Advance(RenderGraph* renderGraph)
{
    _currentIndex = (_currentIndex + 1) % _swapchain->GetImageCount();
   // VkDevice device = _swapchain->GetDevice()->GetRawDevice();
    //VkFence fence = _presentFences[0];
    //vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);
    //vkResetFences(device, 1, &fence);
    
    //std::cout << "AcquireNextImage" << std::endl;
    RenderSynchronizer* synchronizer = renderGraph->GetSynchronizer();

    _queuedImageCount++;
    VkFence fence = VK_NULL_HANDLE;
    if (_queuedImageCount >= _swapchain->GetImageCount())
        fence = synchronizer->GetFence();

    _acquireSemaphore = synchronizer->GetSemaphore();
     _swapchain->AcquireNextImage(UINT64_MAX, _acquireSemaphore, fence);

     if (fence != VK_NULL_HANDLE)
     {
        synchronizer->WaitForFence(fence);
     }
}

void Viewport::Present(RenderGraph* renderGraph)
{
    //TPtr<VulkanImage> currentImage = GetCurrentImage();
    //renderGraph->TransitionLayout(currentImage, VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VkImageLayout::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    //std::cout << "Execute" << std::endl;
    RenderSynchronizer* synchronizer = renderGraph->GetSynchronizer();
    synchronizer->ReturnSemaphore(_acquireSemaphore, renderGraph->GetExecuteCounter());
    VkSemaphore executeSemaphore = synchronizer->GetSemaphore();
    renderGraph->Execute({_acquireSemaphore}, {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT}, {executeSemaphore});

    //std::cout << "Present" << std::endl;
    synchronizer->ReturnSemaphore(executeSemaphore, renderGraph->GetExecuteCounter());
    renderGraph->Present(_swapchain, {executeSemaphore});
    _queuedImageCount--;
}

}