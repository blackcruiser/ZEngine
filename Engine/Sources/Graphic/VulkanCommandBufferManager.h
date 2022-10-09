#pragma once

#include "ZEDefine.h"
#include "ZEType.h"

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

    TPtr<VulkanCommandBuffer> GetCommandBuffer(VulkanQueue::EType type);

private:
    TPtrUnorderedMap<VulkanQueue::EType, VulkanCommandPool> _commandPools;
};

} // namespace ZE
