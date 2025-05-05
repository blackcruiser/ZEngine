#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"

#include <vulkan/vulkan.h>


namespace ZE {

class VulkanDevice
{
public:
    VulkanDevice(VkInstance instance, VkPhysicalDevice physicalDevice);
    ~VulkanDevice();

    void WaitIdle();

    VkDevice GetRawDevice();
    VkInstance GetRawInstance();
    VkPhysicalDevice GetRawPhysicalDevice();

private:
    VkDevice _device;
    VkInstance _instance;
    VkPhysicalDevice _physicalDevice;
};

class VulkanDeviceChild
{
public:
    VulkanDeviceChild(VulkanDevice* device);
    VulkanDeviceChild() = delete;
    
    VulkanDevice* GetDevice();

protected:
    VulkanDevice* _device;
};

} // namespace ZE