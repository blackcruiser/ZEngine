#include "VulkanCommandBufferManager.h"
#include "VulkanCommandBuffer.h"
#include "VulkanSynchronizer.h"
#include "Misc/AssertionMacros.h"


namespace ZE {

VulkanCommandBufferManager::VulkanCommandBufferManager(VulkanDevice* device, uint32_t queueFamilyIndex) :
    VulkanDeviceChild(device), _queueFamilyIndex(queueFamilyIndex)
{
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndex;

    VkResult result = vkCreateCommandPool(device->GetRawDevice(), &poolInfo, nullptr, &_commandPool);
    CHECK_MSG(result == VkResult::VK_SUCCESS, "Create CommandPool fail!");
}

VulkanCommandBufferManager::~VulkanCommandBufferManager()
{
    Recycle();

    for (VulkanCommandBuffer* commandBuffer : _freeCommandBuffers)
    {
        delete commandBuffer;
    }

    vkDestroyCommandPool(_device->GetRawDevice(), _commandPool, nullptr);
}

VulkanCommandBuffer* VulkanCommandBufferManager::Acquire()
{
    Recycle();

    if (_freeCommandBuffers.empty())
    {
        return new VulkanCommandBuffer(_device, _commandPool, _queueFamilyIndex);
    }
    else
    {
        auto iterator = _freeCommandBuffers.begin();
        VulkanCommandBuffer* commandBuffer = *iterator;
        _freeCommandBuffers.erase(iterator);
        return commandBuffer;
    }
}

void VulkanCommandBufferManager::Release(VulkanCommandBuffer* commandBuffer)
{
    _submittedCommandBuffers.push_back(commandBuffer);
}

void VulkanCommandBufferManager::Recycle()
{
    for (auto iterator = _submittedCommandBuffers.begin(); iterator != _submittedCommandBuffers.end(); )
    {
        VulkanCommandBuffer* commandBuffer = *iterator;
        if (IsSignaled(_device, commandBuffer->GetFence()))
        {
            commandBuffer->Reset();
            _freeCommandBuffers.push_back(commandBuffer);
            iterator = _submittedCommandBuffers.erase(iterator);
        }
        else
        {
            iterator++;
        }
    }
}

} // namespace ZE