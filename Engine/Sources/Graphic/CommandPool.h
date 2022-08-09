#pragma once

#include "CoreDefines.h"

#include <vulkan/vulkan.hpp>

#include <set>


namespace TE {

class Device;
class CommandBuffer;

class CommandPool
{
public:
    CommandPool(TPtr<Device> device);
    ~CommandPool();

    CommandBuffer* CreateCommandBuffer(TPtr<CommandPool> commandPool);
    void DestroyCommandBuffer(CommandBuffer* commandBuffer);

    VkCommandPool GetRawCommandPool();
    TPtr<Device> GetDevice();

private:
    void _CreateRawCommandPool();

private:
    TPtr<Device> _device;

    VkCommandPool _vkCommandPool;
    std::set<CommandBuffer*> _commandBuffers;
};

}