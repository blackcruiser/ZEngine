#include "VulkanSwapchain.h"
#include "VulkanGPU.h"
#include "VulkanDevice.h"
#include "VulkanSurface.h"
#include "VulkanImage.h"

#include <glm/vec2.hpp>

#include <stdexcept>
#include <assert.h>


namespace ZE {

TPtr<VulkanSurface> CreateSurface(TPtr<VulkanDevice> device, void* windowHandle, const glm::ivec2& size)
{
    TPtr<VulkanGPU> GPU = device->GetGPU();
    TPtr<VulkanSurface> surface = std::make_shared<VulkanSurface>(GPU->GetVkInstance(), windowHandle);

    VkExtent2D extent{size.x, size.y};
    surface->InitializeExtent(GPU, extent);
    VkSurfaceFormatKHR surfaceFormat{ VkFormat::VK_FORMAT_B8G8R8A8_UNORM, VkColorSpaceKHR::VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
    surface->InitializeFormat(GPU, surfaceFormat);
    surface->InitializePresentMode(GPU, VkPresentModeKHR::VK_PRESENT_MODE_MAILBOX_KHR);

    return surface;
}

VulkanSwapchain::VulkanSwapchain(TPtr<VulkanDevice> device, void* windowHandle, const glm::ivec2& size, uint32_t imageCount)
    : _device(device), _vkSwapchain(VK_NULL_HANDLE), _imageCount(imageCount), _acquiredIndex(-1)
{
    _surface = CreateSurface(device, windowHandle, size);
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

uint32_t VulkanSwapchain::GetImageCount()
{
    return _imageCount;
}

uint32_t VulkanSwapchain::GetCurrentIndex()
{
    return _acquiredIndex;
}

TPtr<VulkanImage> VulkanSwapchain::GetCurrentImage()
{
    assert (_acquiredIndex >= 0);
    
    return _imagerArr[_acquiredIndex];
}

bool VulkanSwapchain::AcquireNextImage(uint64_t timeout, VkSemaphore semaphore, VkFence fence)
{
    VkResult result = vkAcquireNextImageKHR(_device->GetRawDevice(), _vkSwapchain, timeout, semaphore, fence, &_acquiredIndex);

    if (result == VkResult::VK_SUCCESS)
        return true;
    return false;
}

TPtr<VulkanDevice> VulkanSwapchain::GetDevice()
{
    return _device;
}

VkSwapchainKHR VulkanSwapchain::GetRawSwapchain()
{
    return _vkSwapchain;
}

} // namespace ZE
