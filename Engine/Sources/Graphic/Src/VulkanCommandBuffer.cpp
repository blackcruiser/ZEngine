#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"
#include "VulkanCommandPool.h"

#include <stdexcept>


namespace ZE {

VulkanCommandBuffer::VulkanCommandBuffer(TPtr<VulkanCommandPool> commandPool) : _commandPool(commandPool), _vkCommandBuffer(VK_NULL_HANDLE)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = _commandPool->GetRawCommandPool();
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    _vkCommandBuffer;
    VkDevice vkDevice = _commandPool->GetDevice()->GetRawDevice();
    if (vkAllocateCommandBuffers(vkDevice, &allocInfo, &_vkCommandBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

VulkanCommandBuffer::~VulkanCommandBuffer()
{
    VkDevice vkDevice = _commandPool->GetDevice()->GetRawDevice();
    VkCommandPool vkCommandPool = _commandPool->GetRawCommandPool();

    vkFreeCommandBuffers(vkDevice, vkCommandPool, 1, &_vkCommandBuffer);
}

void VulkanCommandBuffer::Begin()
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(_vkCommandBuffer, &beginInfo);
}

void VulkanCommandBuffer::End()
{
    vkEndCommandBuffer(_vkCommandBuffer);
}

VkCommandBuffer VulkanCommandBuffer::GetRawCommandBuffer()
{
    return _vkCommandBuffer;
}

}