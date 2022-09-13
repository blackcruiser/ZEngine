#pragma once

#include "ZEDefine.h"
#include "ZEType.h"

#include <vulkan/vulkan.h>


namespace ZE {

class VulkanDevice;
class VulkanCommandBuffer;

class VulkanCommandPool
{
public:
    VulkanCommandPool(TPtr<VulkanDevice> device);
    ~VulkanCommandPool();

    VkCommandPool GetRawCommandPool();
    TPtr<VulkanDevice> GetDevice();

private:
    TPtr<VulkanDevice> _device;

    VkCommandPool _vkCommandPool;
};

}