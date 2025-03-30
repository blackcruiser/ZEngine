#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"

#include "VulkanQueue.h"


namespace ZE {

class VulkanDevice;
class VulkanCommandPool;
class VulkanCommandBuffer;

class VulkanCommandBufferManager
{
public:
    VulkanCommandBufferManager(TPtr<VulkanDevice> device, TPtrArr<VulkanQueue> queueArr);
    ~VulkanCommandBufferManager();

    TPtr<VulkanCommandBuffer> Acquire(VulkanQueue::EType type);
    void Release(TPtr<VulkanCommandBuffer> commandBuffer);

private:
    TPtrUnorderedMap<VulkanQueue::EType, VulkanCommandPool> _commandPools;

    TPtrArr<VulkanCommandBuffer> _submittedCommandBuffers;
    std::unordered_map<VulkanCommandPool*, TPtrArr<VulkanCommandBuffer>> _freeCommandBuffers;
};

} // namespace ZE
