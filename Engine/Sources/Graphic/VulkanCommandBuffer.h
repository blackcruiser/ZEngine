#pragma once

#include "CoreDefines.h"

#include <vulkan/vulkan.hpp>


namespace TE {

class VulkanDevice;
class VulkanCommandPool;

class VulkanCommandBuffer
{
public:
    VulkanCommandBuffer(TPtr<VulkanCommandPool> commandPool);
    ~VulkanCommandBuffer();

    void Begin();
    void End();

    VkCommandBuffer GetRawCommandBuffer();

private:
    TPtr<VulkanCommandPool> _commandPool;

    VkCommandBuffer _vkCommandBuffer;
};

}