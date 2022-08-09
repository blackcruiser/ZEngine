#pragma once

#include "CoreDefines.h"

#include <vulkan/vulkan.hpp>


namespace TE {

class Device;
class CommandPool;

class CommandBuffer
{
public:
    CommandBuffer(TPtr<CommandPool> commandPool);
    ~CommandBuffer();

    void Begin();
    void End();

    VkCommandBuffer GetRawCommandBuffer();

private:
    void _CreateRawBuffer();
    void _AllocateAndBindBufferMemory();

private:
    TPtr<CommandPool> _commandPool;

    VkCommandBuffer _vkCommandBuffer;
};

}