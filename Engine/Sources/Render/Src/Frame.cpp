#include "Frame.h"
#include "RenderSystem.h"
#include "RenderTargets.h"
#include "Graphic/VulkanDevice.h"
#include "Graphic/VulkanSwapchain.h"
#include "Graphic/VulkanImageView.h"
#include "Graphic/VulkanCommandBufferManager.h"

namespace ZE {

Frame::Frame(TPtr<VulkanDevice> device, TPtr<VulkanSwapchain> swapchain)
    : _cachedDevice(device)
{
    _imageAvailableSemaphore = device->CreateGraphicSemaphore();
    _fence = device->CreateFence(false);

    TPtr<VulkanImage> sceneImage = swapchain->AcquireNextImage(UINT64_MAX, _imageAvailableSemaphore, VK_NULL_HANDLE);
    _renderTarget = std::make_shared<VulkanImageView>(sceneImage);

    _extent = _renderTarget->GetExtent();

    _cachedCommandBuffer = RenderSystem::Get().GetCommandBufferManager()->GetCommandBuffer(VulkanQueue::EType::Graphic);
}

Frame::~Frame()
{
    RenderSystem::Get().GetDevice()->DestroyFence(_fence);
    RenderSystem::Get().GetDevice()->DestroyGraphicSemaphore(_imageAvailableSemaphore);

    for (VkSemaphore sem : _semaphoreArr)
    {
        RenderSystem::Get().GetDevice()->DestroyGraphicSemaphore(sem);
    }
}

TPtr<VulkanImageView> Frame::GetFrameBuffer()
{
    return _renderTarget;
}

VkExtent3D Frame::GetExtent()
{
    return _extent;
}

VkSemaphore Frame::GetAvailableSemaphore()
{
    return _imageAvailableSemaphore;
}

VkFence Frame::GetFence()
{
    return _fence;
}

TPtr<VulkanCommandBuffer> Frame::GetCachedCommandBuffer()
{
    return _cachedCommandBuffer;
}

void Frame::PutImage(TPtr<VulkanImageView> imageView)
{
    _imageViewArr.push_back(imageView);
}

void Frame::PutFramebuffer(TPtr<VulkanFramebuffer> framebuffer)
{
    _framebufferArr.push_back(framebuffer);
}

void Frame::PutSemaphore(VkSemaphore sem)
{
    _semaphoreArr.push_back(sem);
}
} // namespace ZE