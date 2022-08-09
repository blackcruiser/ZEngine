#include "CommandPool.h"
#include "CommandBuffer.h"
#include "Device.h"

namespace TE {

CommandPool::CommandPool(TPtr<Device> device) : _device(device)
{
    _CreateRawCommandPool();
}

CommandPool::~CommandPool()
{


    VkDevice vkDevice = _device->GetRawDevice();
    vkDestroyCommandPool(vkDevice, _vkCommandPool, nullptr);
}

CommandBuffer* CommandPool::CreateCommandBuffer(TPtr<CommandPool> commandPool)
{
    CommandBuffer* commandBuffer = new CommandBuffer(commandPool);

    _commandBuffers.insert(commandBuffer);

    return commandBuffer;
}

void CommandPool::DestroyCommandBuffer(CommandBuffer* commandBuffer)
{
    _commandBuffers.erase(commandBuffer);
    delete commandBuffer;
}


VkCommandPool CommandPool::GetRawCommandPool()
{
    return _vkCommandPool;
}

TPtr<Device> CommandPool::GetDevice()
{
    return _device;
}

void CommandPool::_CreateRawCommandPool()
{
    VkDevice vkDevice = _device->GetRawDevice();
    uint32_t graphicQueueFamilyIndex = _device->GetGraphicQueueFamilyIndex();

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = graphicQueueFamilyIndex;

    if (vkCreateCommandPool(vkDevice, &poolInfo, nullptr, &_vkCommandPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create command pool!");
    }
}

}