#pragma once

#include "ZEDefine.h"
#include "ZEType.h"

#include <vulkan/vulkan.h>


namespace ZE {

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