#pragma once

#include "CoreDefines.h"

#include <vulkan/vulkan.h>


namespace TE {

class Surface;
class Device;

class GPU
{
public:
    GPU(const VkInstance& vkInstance);
    ~GPU();

    VkPhysicalDevice GetRawPhysicalDevice();
    const std::vector<const char*>& GetExtensions();

    std::vector<VkExtensionProperties> GetExtensionProperties(VkPhysicalDevice GPU);
    std::vector<VkQueueFamilyProperties> GetQueueFamilyProperties();
    bool isSurfaceSupported(uint32_t queueFamilyIndex, TPtr<Surface> surface);

    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

private:
    std::vector<VkPhysicalDevice> GetSupportedGPUs();

private:
    VkInstance _vkInstance;
    VkPhysicalDevice _GPU;

    const std::vector<const char*> _deviceExtensions;
};

}