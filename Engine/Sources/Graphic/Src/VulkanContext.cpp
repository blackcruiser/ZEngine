#include "VulkanContext.h"
#include "Misc/AssertionMacros.h"

#include <string>
#include <algorithm>
#include <iterator>


namespace ZE {

VkInstance CreateInstance()
{
    const std::string appName{"ZEngine"};
    
    // Instance
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = appName.c_str();
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = appName.c_str();
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_HEADER_VERSION_COMPLETE;
    
    const std::vector<const char*> extensions = {
        "VK_KHR_surface",
     #ifdef ZE_PLATFORM_WINDOWS
        "VK_KHR_win32_surface",
    #endif
     #ifdef ZE_PLATFORM_MACOS
        VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
        VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME
    #endif
    };
    
    const std::vector<const char*> validationLayers = {
    #ifdef ZE_DEBUG
        "VK_LAYER_KHRONOS_validation"
    #endif
    };
    
    VkInstanceCreateInfo instanceCreateInfo{};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &appInfo;
    instanceCreateInfo.enabledExtensionCount = extensions.size();
    instanceCreateInfo.ppEnabledExtensionNames = extensions.data();
    instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
#ifdef ZE_PLATFORM_MACOS
    instanceCreateInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif
    
    VkInstance instance;
    VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);
    CHECK_MSG(result == VkResult::VK_SUCCESS, "Failed to create vulkan instance!");

    return instance;
}

void DestroyInstance(VkInstance instance)
{
    CHECK_MSG(instance, "instance is empty!");
    vkDestroyInstance(instance, nullptr);
}

const std::vector<const char*>& GetExtensions()
{
    static std::vector<const char*> extensions { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    return extensions;
}

std::vector<VkPhysicalDevice> GetPhysicalDevices(VkInstance instance)
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    return devices;
}

std::vector<VkExtensionProperties> GetExtensionProperties(VkPhysicalDevice physicalDevice)
{
    uint32_t extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

    return availableExtensions;
}

VkPhysicalDevice AcquirePhysicalDevice(VkInstance instance)
{
    // Physical VulkanDevice
    std::vector<VkPhysicalDevice> devices = GetPhysicalDevices(instance);

    const std::vector<const char*>& extensions = GetExtensions();
    std::set<std::string> requiredExtensions(extensions.begin(), extensions.end());

    VkPhysicalDevice selectedDevice = VK_NULL_HANDLE;
    for (VkPhysicalDevice physicalDevice : devices)
    {
        std::vector<VkExtensionProperties> availableExtensions = GetExtensionProperties(physicalDevice);

        uint32_t requiredExtensionCount = 0;
        for (const VkExtensionProperties& extension : availableExtensions)
        {
            std::string name(extension.extensionName);
            if (requiredExtensions.contains(name))
                requiredExtensionCount++;
        }

        if (requiredExtensions.size() == requiredExtensionCount)
        {
            selectedDevice = physicalDevice;
            break;
        }
    }

    return selectedDevice;
}

std::vector<VkQueueFamilyProperties> GetQueueFamilyProperties(VkPhysicalDevice physicalDevice)
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilieProperties(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilieProperties.data());

    return queueFamilieProperties;
}

VkPhysicalDeviceMemoryProperties GetMemoryProperties(VkPhysicalDevice physicalDevice)
{
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

    return memoryProperties;
}

uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties = GetMemoryProperties(physicalDevice);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    CHECK_MSG(false, "Unable to find memory type");
}

}