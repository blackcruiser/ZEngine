#include "VulkanCommandPool.h"
#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"

namespace TE {

VulkanCommandPool::VulkanCommandPool(TPtr<VulkanDevice> device) : _device(device)
{
    _CreateRawCommandPool();
}

VulkanCommandPool::~VulkanCommandPool()
{


    VkDevice vkDevice = _device->GetRawDevice();
    vkDestroyCommandPool(vkDevice, _vkCommandPool, nullptr);
}

VulkanCommandBuffer* VulkanCommandPool::CreateCommandBuffer(TPtr<VulkanCommandPool> commandPool)
{
    VulkanCommandBuffer* commandBuffer = new VulkanCommandBuffer(commandPool);

    _commandBuffers.insert(commandBuffer);

    return commandBuffer;
}

void VulkanCommandPool::DestroyCommandBuffer(VulkanCommandBuffer* commandBuffer)
{
    _commandBuffers.erase(commandBuffer);
    delete commandBuffer;
}


VkCommandPool VulkanCommandPool::GetRawCommandPool()
{
    return _vkCommandPool;
}

TPtr<VulkanDevice> VulkanCommandPool::GetDevice()
{
    return _device;
}

void VulkanCommandPool::_CreateRawCommandPool()
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