#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"

#include <vulkan/vulkan.h>


namespace ZE {

class Surface;
class VulkanDevice;

class VulkanGPU
{
public:
    VulkanGPU(const VkInstance& vkInstance);
    ~VulkanGPU();

    VkPhysicalDevice GetRawGPU();
    const std::vector<const char*>& GetExtensions();

    std::vector<VkExtensionProperties> GetExtensionProperties(VkPhysicalDevice GPU);
    std::vector<VkQueueFamilyProperties> GetQueueFamilyProperties();
    bool isSurfaceSupported(uint32_t queueFamilyIndex, TPtr<Surface> surface);

    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

private:
    std::vector<VkPhysicalDevice> GetSupportedRawGPUs();

private:
    VkInstance _vkInstance;
    VkPhysicalDevice _GPU;

    const std::vector<const char*> _deviceExtensions;
};

}