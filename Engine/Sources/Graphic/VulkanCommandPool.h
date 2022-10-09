#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"

#include <vulkan/vulkan.h>


namespace ZE {

class VulkanDevice;
class VulkanCommandBuffer;

class VulkanCommandPool
{
public:
    VulkanCommandPool(TPtr<VulkanDevice> device, uint32_t queueFamilyIndex);
    ~VulkanCommandPool();

    VkCommandPool GetRawCommandPool();
    TPtr<VulkanDevice> GetDevice();

private:
    TPtr<VulkanDevice> _device;

    uint32_t _queueFamilyIndex;
    VkCommandPool _vkCommandPool;
};

}