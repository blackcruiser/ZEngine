#include "VulkanDevice.h"
#include "VulkanGPU.h"

#include <string>
#include <stdexcept>


namespace ZE {

VulkanDevice::VulkanDevice(TPtr<VulkanGPU> GPU)
    : _GPU(GPU), _vkDevice(VK_NULL_HANDLE), _graphicQueueFamilyIndex(-1),
      _computeQueueFamilyIndex(-1), _transferQueueFamilyIndex(-1)
{
    // Queue
    std::vector<VkQueueFamilyProperties> queueFamilyProperties = _GPU->GetQueueFamilyProperties();
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    uint32_t queueCount = 0;

    for (size_t i = 0; i < queueFamilyProperties.size(); i++)
    {
        VkQueueFamilyProperties properties = queueFamilyProperties[i];
        bool isFound = false;

        if ((properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) == VK_QUEUE_GRAPHICS_BIT)
        {
            _graphicQueueFamilyIndex = i;
            isFound = true;
        }

        if ((properties.queueFlags & VK_QUEUE_COMPUTE_BIT) == VK_QUEUE_COMPUTE_BIT)
        {
            _computeQueueFamilyIndex = i;
            isFound = true;
        }

        if ((properties.queueFlags & VK_QUEUE_TRANSFER_BIT) == VK_QUEUE_TRANSFER_BIT)
        {
            _transferQueueFamilyIndex = i;
            isFound = true;
        }

        if (isFound)
        {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = i;
            queueCreateInfo.queueCount = properties.queueCount;

            queueCreateInfos.push_back(queueCreateInfo);
            queueCount += properties.queueCount;
        }
    }

    if (_computeQueueFamilyIndex == -1)
        _computeQueueFamilyIndex = _graphicQueueFamilyIndex;

    if (_transferQueueFamilyIndex == -1)
        _transferQueueFamilyIndex = _computeQueueFamilyIndex;

    std::vector<float> priorityArr(queueCount, 1.0f);
    float* priorityPtr = priorityArr.data();
    for (VkDeviceQueueCreateInfo& createInfo : queueCreateInfos)
    {
        createInfo.pQueuePriorities = priorityPtr;
        priorityPtr++;
    }


    // Extensions
    std::vector<const char*> deviceExtensions = _GPU->GetExtensions();
#ifdef ZE_PLATFORM_MACOS
    deviceExtensions.push_back("VK_KHR_portability_subset");
#endif

    // Features
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

uint32_t VulkanDevice::GetGraphicQueueFamilyIndex()
{
    return _graphicQueueFamilyIndex;
}

uint32_t VulkanDevice::GetComputeQueueFamilyIndex()
{
    return _computeQueueFamilyIndex;
}

uint32_t VulkanDevice::GetTransferQueueFamilyIndex()
{
    return _transferQueueFamilyIndex;
}
} // namespace ZE