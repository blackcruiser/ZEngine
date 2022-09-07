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