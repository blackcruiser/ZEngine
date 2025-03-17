#include "VulkanDevice.h"
#include "VulkanGPU.h"
#include "Surface.h"

#include <array>
#include <limits>
#include <set>
#include <stdexcept>
#include <string>

namespace TE {

VulkanDevice::VulkanDevice(TPtr<VulkanGPU> GPU, TPtr<Surface> surface)
    : _GPU(GPU), _surface(surface), _vkDevice(VK_NULL_HANDLE), _vkGraphicQueue(VK_NULL_HANDLE),
      _vkPresentQueue(VK_NULL_HANDLE), _graphicQueueFamilyIndex(std::numeric_limits<uint32_t>::max()),
      _presentQueueFamilyIndex(std::numeric_limits<uint32_t>::max())
{
    // Queue
    std::vector<VkQueueFamilyProperties> queueFamilyProperties = _GPU->GetQueueFamilyProperties();
    for (size_t i = 0; i < queueFamilyProperties.size(); i++)
    {
        if (_graphicQueueFamilyIndex == std::numeric_limits<uint32_t>::max() &&
            queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            _graphicQueueFamilyIndex = static_cast<uint32_t>(i);

        if (_presentQueueFamilyIndex == std::numeric_limits<uint32_t>::max() && _GPU->isSurfaceSupported(i, _surface))
            _presentQueueFamilyIndex = static_cast<uint32_t>(i);
    }

    if (_graphicQueueFamilyIndex >= queueFamilyProperties.size())
    {
        throw std::runtime_error("can't find Device Queue!");
    }

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {_graphicQueueFamilyIndex, _presentQueueFamilyIndex};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    const std::vector<const char*> deviceExtensions = _GPU->GetExtensions();
    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo vkDeviceCreateInfo{};
    vkDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    vkDeviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    vkDeviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    vkDeviceCreateInfo.pEnabledFeatures = &deviceFeatures;
    vkDeviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
    vkDeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());

    if (vkCreateDevice(_GPU->GetRawGPU(), &vkDeviceCreateInfo, nullptr, &_vkDevice) != VkResult::VK_SUCCESS)
    {
        throw std::runtime_error("create device fail!");
    }

    // Queue
    vkGetDeviceQueue(_vkDevice, _graphicQueueFamilyIndex, 0, &_vkGraphicQueue);
    vkGetDeviceQueue(_vkDevice, _presentQueueFamilyIndex, 0, &_vkPresentQueue);
}

VulkanDevice::~VulkanDevice()
{
    if (_vkDevice != VK_NULL_HANDLE)
        vkDestroyDevice(_vkDevice, nullptr);
}

VkSemaphore VulkanDevice::CreateGraphicSemaphore()
{
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkSemaphore semaphore;
    if (vkCreateSemaphore(_vkDevice, &semaphoreInfo, nullptr, &semaphore) != VK_SUCCESS)

    {
        throw std::runtime_error("failed to create semaphore!");
    }

    return semaphore;
}

void VulkanDevice::DestroyGraphicSemaphore(VkSemaphore semaphore)
{
    vkDestroySemaphore(_vkDevice, semaphore, nullptr);
}

VkFence VulkanDevice::CreateFence(bool isSignaled)
{
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = isSignaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

    VkFence fence;
    if (vkCreateFence(_vkDevice, &fenceInfo, nullptr, &fence) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create fence!");
    }

    return fence;
}

void VulkanDevice::DestroyFence(VkFence fence)
{
    vkDestroyFence(_vkDevice, fence, nullptr);
}


VkSampler VulkanDevice::CreateTextureSampler()
{
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(_GPU->GetRawGPU(), &properties);

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    VkSampler sampler;
    if (vkCreateSampler(_vkDevice, &samplerInfo, nullptr, &sampler) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create texture sampler!");
    }
    return sampler;
}

void VulkanDevice::DestroyTextureSampler(VkSampler sampler)
{
    vkDestroySampler(_vkDevice, sampler, nullptr);
}

VkImageView VulkanDevice::CreateImageView(VkImage image, VkFormat format)
{
    VkImageViewCreateInfo imageViewCreateInfo{};
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.image = image;
    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.format = format;
    imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = 1;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (vkCreateImageView(_vkDevice, &imageViewCreateInfo, nullptr, &imageView) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create image views!");
    }

    return imageView;
}

void VulkanDevice::DestroyImageView(VkImageView imageView)
{
    vkDestroyImageView(_vkDevice, imageView, nullptr);
}

VkFramebuffer VulkanDevice::CreateFramebuffer(VkRenderPass renderPass, const std::vector<VkImageView> imageViewArr,
                                              uint32_t width, uint32_t height)
{
    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass;
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = imageViewArr.data();
    framebufferInfo.width = width;
    framebufferInfo.height = height;
    framebufferInfo.layers = 1;

    VkFramebuffer framebuffer;
    if (vkCreateFramebuffer(_vkDevice, &framebufferInfo, nullptr, &framebuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create framebuffer!");
    }
    return framebuffer;
}

void VulkanDevice::DestroyFramebuffer(VkFramebuffer framebuffer)
{
    vkDestroyFramebuffer(_vkDevice, framebuffer, nullptr);
}

VkSwapchainKHR VulkanDevice::CreateSwapchain(uint32_t imageCount, VkFormat imageFormat, VkColorSpaceKHR colorSpace,
                                             VkExtent2D extent, VkSurfaceTransformFlagBitsKHR preTransform,
                                             VkPresentModeKHR presentMode)
{
    VkSwapchainCreateInfoKHR vkSwapchainCreateInfo{};
    vkSwapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    vkSwapchainCreateInfo.surface = _surface->GetRawSurface();
    vkSwapchainCreateInfo.minImageCount = imageCount;
    vkSwapchainCreateInfo.imageFormat = imageFormat;
    vkSwapchainCreateInfo.imageColorSpace = colorSpace;
    vkSwapchainCreateInfo.imageExtent = extent;
    vkSwapchainCreateInfo.imageArrayLayers = 1;
    vkSwapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    if (_graphicQueueFamilyIndex != _presentQueueFamilyIndex)
    {
        uint32_t queueFamilyIndexs[] = {_graphicQueueFamilyIndex, _presentQueueFamilyIndex};
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
    vkSwapchainCreateInfo.preTransform = preTransform;
    vkSwapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    vkSwapchainCreateInfo.presentMode = presentMode;
    vkSwapchainCreateInfo.clipped = VK_TRUE;
    vkSwapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    VkSwapchainKHR swapchain;
    if (vkCreateSwapchainKHR(_vkDevice, &vkSwapchainCreateInfo, nullptr, &swapchain) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create swap chain!");
    }

    return swapchain;
}

void VulkanDevice::DestroySwapchain(VkSwapchainKHR swapchain)
{
    vkDestroySwapchainKHR(_vkDevice, swapchain, nullptr);
}


VkRenderPass VulkanDevice::CreateRenderPass(VkFormat format)
{
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = format;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    VkRenderPass renderPass;
    if (vkCreateRenderPass(_vkDevice, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create render pass!");
    }

    return renderPass;
}

void VulkanDevice::DestroyRenderPass(VkRenderPass renderPass)
{
    vkDestroyRenderPass(_vkDevice, renderPass, nullptr);
}

VkDescriptorSetLayout VulkanDevice::CreateDescriptorSetLayout(
    const std::vector<VkDescriptorSetLayoutBinding>& layoutBindings)
{
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = layoutBindings.size();
    layoutInfo.pBindings = layoutBindings.data();

    VkDescriptorSetLayout descriptorSetLayout;
    if (vkCreateDescriptorSetLayout(_vkDevice, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor set layout!");
    }

    return descriptorSetLayout;
}

void VulkanDevice::DestroyDescriptorSetLayout(VkDescriptorSetLayout descriptorSetLayout)
{
    vkDestroyDescriptorSetLayout(_vkDevice, descriptorSetLayout, nullptr);
}

void VulkanDevice::WaitIdle()
{
    vkDeviceWaitIdle(_vkDevice);
}

TPtr<VulkanGPU> VulkanDevice::GetGPU()
{
    return _GPU;
}

VkDevice VulkanDevice::GetRawDevice()
{
    return _vkDevice;
}

VkQueue VulkanDevice::GetGraphicQueue()
{
    return _vkGraphicQueue;
}

VkQueue VulkanDevice::GetPresentQueue()
{
    return _vkPresentQueue;
}

uint32_t VulkanDevice::GetGraphicQueueFamilyIndex()
{
    return _graphicQueueFamilyIndex;
}

uint32_t VulkanDevice::GetPresentQueueFamilyIndex()
{
    return _presentQueueFamilyIndex;
}
} // namespace TE