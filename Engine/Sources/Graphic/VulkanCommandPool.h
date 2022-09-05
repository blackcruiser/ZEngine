#pragma once

#include "CoreDefines.h"

#include <vulkan/vulkan.hpp>

#include <set>


namespace TE {

class VulkanDevice;
class VulkanCommandBuffer;

class VulkanCommandPool
{
public:
    VulkanCommandPool(TPtr<VulkanDevice> device);
    ~VulkanCommandPool();

    VulkanCommandBuffer* CreateCommandBuffer(TPtr<VulkanCommandPool> commandPool);
    void DestroyCommandBuffer(VulkanCommandBuffer* commandBuffer);

    VkCommandPool GetRawCommandPool();
    TPtr<VulkanDevice> GetDevice();

private:
    void _CreateRawCommandPool();

private:
    TPtr<VulkanDevice> _device;

    VkCommandPool _vkCommandPool;
    std::set<VulkanCommandBuffer*> _commandBuffers;
};

}