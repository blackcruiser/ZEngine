#include "VulkanSwapchain.h"
#include "VulkanSurface.h"
#include "VulkanImage.h"
#include "Misc/AssertionMacros.h"

#include <glm/vec2.hpp>


namespace ZE {

VulkanSurface* CreateSurface(VulkanDevice* device, void* windowHandle, const glm::ivec2& size)
{
    VulkanSurface* surface = new VulkanSurface(device->GetRawInstance(), windowHandle);

    VkExtent2D extent{size.x, size.y};
    surface->InitializeExtent(device->GetRawPhysicalDevice(), extent);
    VkSurfaceFormatKHR surfaceFormat{ VkFormat::VK_FORMAT_B8G8R8A8_UNORM, VkColorSpaceKHR::VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
    surface->InitializeFormat(device->GetRawPhysicalDevice(), surfaceFormat);
    surface->InitializePresentMode(device->GetRawPhysicalDevice(), VkPresentModeKHR::VK_PRESENT_MODE_MAILBOX_KHR);

    return surface;
}

VulkanSwapchain::VulkanSwapchain(VulkanDevice* device, void* windowHandle, const glm::ivec2& size, uint32_t imageCount)
    : VulkanDeviceChild(device), _swapchain(VK_NULL_HANDLE), _imageCount(imageCount), _acquiredIndex(-1)
{
    _surface = CreateSurface(device, windowHandle, size);
    VkSurfaceFormatKHR surfaceFormat = _surface->GetSurfaceFormat();
    VkPresentModeKHR vkPresentMode = _surface->GetPresentMode();
    VkSurfaceCapabilitiesKHR vkCapabilities = _surface->GetCpabilities(device->GetRawPhysicalDevice());
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

    VkResult result = vkCreateSwapchainKHR(_device->GetRawDevice(), &vkSwapchainCreateInfo, nullptr, &_swapchain);
    CHECK_MSG(result == VkResult::VK_SUCCESS, "Failed to create Swapchain!");

    // Get Images
    uint32_t count = 0;
    vkGetSwapchainImagesKHR(_device->GetRawDevice(), _swapchain, &count, nullptr);

    std::vector<VkImage> vkImages(count);
    vkGetSwapchainImagesKHR(_device->GetRawDevice(), _swapchain, &count, vkImages.data());

    VkExtent3D extent3D{extent.width, extent.height, 1};
    for (size_t i = 0; i < count; i++)
    {
        VulkanImage* vulkanImage = new VulkanImage(_device, vkImages[i], extent3D, surfaceFormat.format);
        _imagerArr.push_back(vulkanImage);
    }
}

VulkanSwapchain::~VulkanSwapchain()
{
    CHECK(_swapchain != VK_NULL_HANDLE);
    vkDestroySwapchainKHR(_device->GetRawDevice(), _swapchain, nullptr);
}

uint32_t VulkanSwapchain::GetImageCount()
{
    return _imageCount;
}

uint32_t VulkanSwapchain::GetCurrentIndex()
{
    return _acquiredIndex;
}

VulkanImage* VulkanSwapchain::GetCurrentImage()
{
    CHECK(_acquiredIndex >= 0);
    
    return _imagerArr[_acquiredIndex];
}

bool VulkanSwapchain::AcquireNextImage(uint64_t timeout, VkSemaphore semaphore, VkFence fence)
{
    VkResult result = vkAcquireNextImageKHR(_device->GetRawDevice(), _swapchain, timeout, semaphore, fence, &_acquiredIndex);

    if (result == VkResult::VK_SUCCESS)
        return true;
    return false;
}

VkSwapchainKHR VulkanSwapchain::GetRawSwapchain()
{
    return _swapchain;
}

} // namespace ZE
