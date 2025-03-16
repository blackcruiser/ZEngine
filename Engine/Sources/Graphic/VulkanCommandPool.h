#pragma once

#include "CoreDefines.h"

#include <vulkan/vulkan.hpp>


namespace TE {

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