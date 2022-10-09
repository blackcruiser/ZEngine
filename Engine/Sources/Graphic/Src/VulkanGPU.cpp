#include "VulkanGPU.h"
#include "VulkanSurface.h"
#include "VulkanDevice.h"

#include <stdexcept>
#include <set>
#include <string>


namespace ZE {

VulkanGPU::VulkanGPU(const VkInstance& vkInstance)
    : _vkInstance(vkInstance),
      _GPU(VK_NULL_HANDLE), _deviceExtensions({VK_KHR_SWAPCHAIN_EXTENSION_NAME})
{
    std::vector<VkPhysicalDevice> GPUs = GetSupportedRawGPUs();

    _GPU = VK_NULL_HANDLE;
    for (size_t i = 0; i < GPUs.size(); i++)
    {
        std::vector<VkExtensionProperties> availableExtensions = GetExtensionProperties(GPUs[i]);

        std::set<std::string> requiredExtensions(_deviceExtensions.begin(), _deviceExtensions.end());

        for (const auto& extension : availableExtensions)
        {
            std::string name(extension.extensionName);
            requiredExtensions.erase(name);
        }

        if (requiredExtensions.empty())
        {
            _GPU = GPUs[i];
            break;
        }
    }
}

VulkanGPU::~VulkanGPU()
{
}

const std::vector<const char*>& VulkanGPU::GetExtensions()
{
    return _deviceExtensions;
}

VkPhysicalDevice VulkanGPU::GetRawGPU()
{
    return _GPU;
}

VkInstance VulkanGPU::GetVkInstance()
{
    return _vkInstance;
}

std::vector<VkExtensionProperties> VulkanGPU::GetExtensionProperties(VkPhysicalDevice GPU)
{
    uint32_t extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(GPU, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(GPU, nullptr, &extensionCount, availableExtensions.data());

    return availableExtensions;
}

std::vector<VkQueueFamilyProperties> VulkanGPU::GetQueueFamilyProperties()
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(_GPU, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilieProperties(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(_GPU, &queueFamilyCount, queueFamilieProperties.data());

    return queueFamilieProperties;
}

bool VulkanGPU::isSurfaceSupported(uint32_t queueFamilyIndex, TPtr<VulkanSurface> surface)
{
    VkBool32 isSupported;
    vkGetPhysicalDeviceSurfaceSupportKHR(_GPU, queueFamilyIndex, surface->GetRawSurface(), &isSupported);
    return isSupported != 0;
}

uint32_t VulkanGPU::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(_GPU, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

std::vector<VkPhysicalDevice> VulkanGPU::GetSupportedRawGPUs()
{
    // Physical VulkanDevice
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(_vkInstance, &deviceCount, nullptr);
    if (deviceCount == 0)
    {
        throw std::runtime_error("no physiccal device!");
    }
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(_vkInstance, &deviceCount, devices.data());

    return devices;
}

} // namespace ZE
