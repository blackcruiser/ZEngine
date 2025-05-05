#include "VulkanDevice.h"
#include "VulkanContext.h"
#include "Misc/AssertionMacros.h"

#include <string>


namespace ZE {

VulkanDevice::VulkanDevice(VkInstance instance, VkPhysicalDevice physicalDevice)
    : _device(VK_NULL_HANDLE), _instance(instance), _physicalDevice(physicalDevice)
{
    // Queue
    std::vector<VkQueueFamilyProperties> queueFamilyProperties = GetQueueFamilyProperties(physicalDevice);
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    uint32_t queueCount = 0;

    for (size_t i = 0; i < queueFamilyProperties.size(); i++)
    {
        VkQueueFamilyProperties properties = queueFamilyProperties[i];

        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = i;
        queueCreateInfo.queueCount = properties.queueCount;

        queueCreateInfos.push_back(queueCreateInfo);
        queueCount += properties.queueCount;
    }

    std::vector<float> priorityArr(queueCount, 1.0f);
    float* priorityPtr = priorityArr.data();
    for (VkDeviceQueueCreateInfo& createInfo : queueCreateInfos)
    {
        createInfo.pQueuePriorities = priorityPtr;
        priorityPtr++;
    }

    // Extensions
    const std::vector<const char*>& deviceExtensions = GetExtensions();
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

    VkResult result = vkCreateDevice(physicalDevice, &vkDeviceCreateInfo, nullptr, &_device);
    CHECK_MSG(result == VkResult::VK_SUCCESS, "Create device fail!");
}

VulkanDevice::~VulkanDevice()
{
    CHECK(_device != VK_NULL_HANDLE);
    vkDestroyDevice(_device, nullptr);
}

void VulkanDevice::WaitIdle()
{
    vkDeviceWaitIdle(_device);
}

VkDevice VulkanDevice::GetRawDevice()
{
    return _device;
}

VkInstance VulkanDevice::GetRawInstance()
{
    return _instance;
}

VkPhysicalDevice VulkanDevice::GetRawPhysicalDevice()
{
    return _physicalDevice;
}


VulkanDeviceChild::VulkanDeviceChild(VulkanDevice* device)
    : _device(device)
{
}

VulkanDevice* VulkanDeviceChild::GetDevice()
{
    return _device;
}

} // namespace ZE