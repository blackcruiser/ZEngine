#include "VulkanCommandBufferManager.h"
#include "VulkanCommandBuffer.h"
#include "VulkanSynchronizer.h"
#include "Debug/AssertionMacros.h"


namespace ZE {

VulkanCommandBufferManager::VulkanCommandBufferManager(VulkanDevice* device, uint32_t queueFamilyIndex) :
    VulkanDeviceChild(device), _queueFamilyIndex(queueFamilyIndex)
{
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndex;

    VkResult result = vkCreateCommandPool(device->GetRawDevice(), &poolInfo, nullptr, &_commandPool);
    ZE_CHECK_MSG(result == VkResult::VK_SUCCESS, "Create CommandPool fail!");
}

VulkanCommandBufferManager::~VulkanCommandBufferManager()
{
    for (auto& entry : _submittedCommandBuffers)
    {
        VulkanCommandBuffer* commandBuffer = std::get<0>(entry);
        delete commandBuffer;
    }
    _submittedCommandBuffers.clear();

    for (VulkanCommandBuffer* commandBuffer : _freeCommandBuffers)
    {
        delete commandBuffer;
    }
    _freeCommandBuffers.clear();

    vkDestroyCommandPool(_device->GetRawDevice(), _commandPool, nullptr);
}

VulkanCommandBuffer* VulkanCommandBufferManager::Acquire()
{
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

void VulkanCommandBufferManager::Release(VulkanCommandBuffer* commandBuffer, uint32 frameCount)
{
    _submittedCommandBuffers.emplace_back(commandBuffer, frameCount);
}

void VulkanCommandBufferManager::Recycle(uint32 safeFrameCount)
{
    for (auto iterator = _submittedCommandBuffers.begin(); iterator != _submittedCommandBuffers.end(); )
    {
        VulkanCommandBuffer* commandBuffer = std::get<0>(*iterator);
        uint32 frameCount = std::get<1>(*iterator);
        if (frameCount <= safeFrameCount)
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