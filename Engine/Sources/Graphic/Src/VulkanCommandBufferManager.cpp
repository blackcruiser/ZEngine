#include "VulkanCommandBufferManager.h"
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

TPtr<VulkanCommandBuffer> VulkanCommandBufferManager::GetCommandBuffer(VulkanQueue::EType type)
{
    if (_commandPools.find(type) == _commandPools.end())
        return nullptr;
    else
    {
        TPtr<VulkanCommandPool> commandPool = _commandPools[type];
        return std::make_shared<VulkanCommandBuffer>(commandPool);
    }
}

} // namespace ZE