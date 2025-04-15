#include "VulkanCommandBufferManager.h"
#include "VulkanDevice.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandBuffer.h"


namespace ZE {

VulkanCommandBufferManager::VulkanCommandBufferManager(TPtr<VulkanDevice> device, TPtrArr<VulkanQueue> queueArr)
{
    for (const TPtr<VulkanQueue>& queue : queueArr)
    {
        TPtr<VulkanCommandPool> commandPool = std::make_shared<VulkanCommandPool>(device, queue->GetFamilyIndex());
        _commandPools.insert(std::make_pair(queue->GetType(), commandPool));
    }
}

VulkanCommandBufferManager::~VulkanCommandBufferManager()
{
}

TPtr<VulkanCommandBuffer> VulkanCommandBufferManager::Acquire(VulkanQueue::EType type)
{
    if (_commandPools.find(type) == _commandPools.end())
        return nullptr;

    for (auto iterator = _submittedCommandBuffers.begin(); iterator != _submittedCommandBuffers.end(); )
    {
        TPtr<VulkanCommandBuffer>& commandBuffer = *iterator;
        if (commandBuffer->GetDevice()->IsSignaled(commandBuffer->GetFence()))
        {
            commandBuffer->Reset();
            _freeCommandBuffers[commandBuffer->GetCommandPool().get()].push_back(commandBuffer);
            iterator = _submittedCommandBuffers.erase(iterator);
        }
        else
        {
            iterator++;
        }
    }

    if (_freeCommandBuffers.empty() || _freeCommandBuffers[_commandPools[type].get()].empty())
    {
        TPtr<VulkanCommandPool> commandPool = _commandPools[type];
        return std::make_shared<VulkanCommandBuffer>(commandPool);
    }
    else
    {
        auto iterator = _freeCommandBuffers[_commandPools[type].get()].begin();
        TPtr<VulkanCommandBuffer> commandBuffer = *iterator;
        _freeCommandBuffers[_commandPools[type].get()].erase(iterator);
        return commandBuffer;
    }
}

void VulkanCommandBufferManager::Release(TPtr<VulkanCommandBuffer> commandBuffer)
{
    _submittedCommandBuffers.push_back(commandBuffer);
}

} // namespace ZE