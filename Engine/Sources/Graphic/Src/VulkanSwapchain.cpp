#include "VulkanSwapchain.h"
#include "VulkanDevice.h"
#include "VulkanSurface.h"
#include "VulkanImage.h"

#include <stdexcept>
#include <assert.h>


namespace ZE {

VulkanSwapchain::VulkanSwapchain(TPtr<VulkanDevice> device, TPtr<VulkanSurface> surface, uint32_t imageCount)
    : _device(device), _surface(surface), _vkSwapchain(VK_NULL_HANDLE), _acquiredIndex(-1)
{
    VkSurfaceFormatKHR surfaceFormat = _surface->GetSurfaceFormat();
    VkPresentModeKHR vkPresentMode = _surface->GetPresentMode();
    VkSurfaceCapabilitiesKHR vkCapabilities = _surface->GetCpabilities(_device->GetGPU());
    VkExtent2D extent = _surface->GetExtent();

    // Create
    VkSwapchainCreateInfoKHR vkSwapchainCreateInfo{};
    vkSwapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    vkSwapchainCreateInfo.surface = _surface->GetRawSurface();
    vkSwapchainCreateInfo.minImageCount = imageCount;
    vkSwapchainCreateInfo.imageFormat = surfaceFormat.format;
    vkSwapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    vkSwapchainCreateInfo.imageExtent = _surface->GetExtent();
    vkSwapchainCreateInfo.imageArrayLayers = 1;
    vkSwapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    vkSwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    vkSwapchainCreateInfo.queueFamilyIndexCount = 0;     // Optional
    vkSwapchainCreateInfo.pQueueFamilyIndices = nullptr; // Optional

    vkSwapchainCreateInfo.preTransform = vkCapabilities.currentTransform;
    vkSwapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    vkSwapchainCreateInfo.presentMode = vkPresentMode;
    vkSwapchainCreateInfo.clipped = VK_TRUE;
    vkSwapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(_device->GetRawDevice(), &vkSwapchainCreateInfo, nullptr, &_vkSwapchain) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create swap chain!");
    }


    // Get Images
    uint32_t count = 0;
    vkGetSwapchainImagesKHR(_device->GetRawDevice(), _vkSwapchain, &count, nullptr);

    std::vector<VkImage> vkImages(count);
    vkGetSwapchainImagesKHR(_device->GetRawDevice(), _vkSwapchain, &count, vkImages.data());

    TPtrArr<VulkanImage> vulkanImages(count);
    VkExtent3D extent3D{extent.width, extent.height, 1};
    for (size_t i = 0; i < count; i++)
    {
        TPtr<VulkanImage> vulkanImage = std::make_shared<VulkanImage>(_device, vkImages[i], extent3D, surfaceFormat.format);
        vulkanImages[i] = vulkanImage;
    }
    _imagerArr.swap(vulkanImages);
}

VulkanSwapchain::~VulkanSwapchain()
{
    if (_vkSwapchain != VK_NULL_HANDLE)
        vkDestroySwapchainKHR(_device->GetRawDevice(), _vkSwapchain, nullptr);
}

uint32_t VulkanSwapchain::GetCurrentAcquiredIndex()
{
    assert (_acquiredIndex >= 0);
    
    return _acquiredIndex;
}

TPtr<VulkanImage> VulkanSwapchain::AcquireNextImage(uint64_t timeout, VkSemaphore semaphore, VkFence fence)
{
    VkResult result = vkAcquireNextImageKHR(_device->GetRawDevice(), _vkSwapchain, timeout, semaphore, fence, &_acquiredIndex);

    if (result == VkResult::VK_SUCCESS)
        return _imagerArr[_acquiredIndex];
    else
        return nullptr;
}

VkSwapchainKHR VulkanSwapchain::GetRawSwapchain()
{
    return _vkSwapchain;
}


} // namespace ZE
