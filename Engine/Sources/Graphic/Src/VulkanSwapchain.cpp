#include "VulkanSwapchain.h"
#include "VulkanDevice.h"
#include "Surface.h"
#include "VulkanImage.h"

#include <stdexcept>


namespace ZE {

VulkanSwapchain::VulkanSwapchain(TPtr<VulkanDevice> device, TPtr<Surface> surface, uint32_t imageCount)
    : _device(device), _surface(surface)
{
    VkSurfaceFormatKHR surfaceFormat = _surface->GetSurfaceFormat();
    VkPresentModeKHR vkPresentMode = _surface->GetPresentMode();
    VkSurfaceCapabilitiesKHR vkCapabilities = _surface->GetCpabilities();

    _extent = _surface->GetExtent();
    _format = surfaceFormat.format;

    VkSwapchainCreateInfoKHR vkSwapchainCreateInfo{};
    vkSwapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    vkSwapchainCreateInfo.surface = _surface->GetRawSurface();
    vkSwapchainCreateInfo.minImageCount = imageCount;
    vkSwapchainCreateInfo.imageFormat = _format;
    vkSwapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    vkSwapchainCreateInfo.imageExtent = _extent;
    vkSwapchainCreateInfo.imageArrayLayers = 1;
    vkSwapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    if (_device->GetGraphicQueueFamilyIndex() != _device->GetPresentQueueFamilyIndex())
    {
        uint32_t queueFamilyIndexs[] = {_device->GetGraphicQueueFamilyIndex(), _device->GetPresentQueueFamilyIndex()};
        vkSwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        vkSwapchainCreateInfo.queueFamilyIndexCount = 2;
        vkSwapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndexs;
    }
    else
    {
        vkSwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        vkSwapchainCreateInfo.queueFamilyIndexCount = 0;     // Optional
        vkSwapchainCreateInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    vkSwapchainCreateInfo.preTransform = vkCapabilities.currentTransform;
    vkSwapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    vkSwapchainCreateInfo.presentMode = vkPresentMode;
    vkSwapchainCreateInfo.clipped = VK_TRUE;
    vkSwapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(_device->GetRawDevice(), &vkSwapchainCreateInfo, nullptr, &_vkSwapchain) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create swap chain!");
    }
}

VulkanSwapchain::~VulkanSwapchain()
{
    if (_vkSwapchain != VK_NULL_HANDLE)
        vkDestroySwapchainKHR(_device->GetRawDevice(), _vkSwapchain, nullptr);
}

TPtrArr<VulkanImage> VulkanSwapchain::GetImages()
{
    uint32_t count = 0;
    vkGetSwapchainImagesKHR(_device->GetRawDevice(), _vkSwapchain, &count, nullptr);

    std::vector<VkImage> vkImages(count);
    vkGetSwapchainImagesKHR(_device->GetRawDevice(), _vkSwapchain, &count, vkImages.data());

    TPtrArr<VulkanImage> vulkanImages(count);
    VkExtent3D extent3D{_extent.width, _extent.height, 1};
    for (size_t i = 0; i < count; i++)
    {
        TPtr<VulkanImage> vulkanImage = std::make_shared<VulkanImage>(_device, vkImages[i], extent3D, _format);
        vulkanImages[i] = vulkanImage;
    }

    return vulkanImages;
}

uint32_t VulkanSwapchain::AcquireNextImage(VkSemaphore semaphore)
{
    uint32_t imageIndex = 0;
    vkAcquireNextImageKHR(_device->GetRawDevice(), _vkSwapchain, UINT64_MAX, semaphore, VK_NULL_HANDLE, &imageIndex);

    return imageIndex;
}

VkSwapchainKHR VulkanSwapchain::GetRawSwapchain()
{
    return _vkSwapchain;
}


} // namespace ZE
