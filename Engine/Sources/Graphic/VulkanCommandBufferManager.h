#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"

#include "VulkanQueue.h"


namespace ZE {

class VulkanDevice;
class VulkanCommandBuffer;

class VulkanCommandBufferManager : public VulkanDeviceChild
{
public:
    VulkanCommandBufferManager(VulkanDevice* device, uint32_t queueFamilyIndex);
    ~VulkanCommandBufferManager();

    VulkanCommandBuffer* Acquire();
    void Release(VulkanCommandBuffer* commandBuffer);

    void Recycle();

private:
    uint32_t _queueFamilyIndex;
    VkCommandPool _commandPool;

    std::vector<VulkanCommandBuffer*> _submittedCommandBuffers;
    std::vector<VulkanCommandBuffer*>  _freeCommandBuffers;
};

} // namespace ZE
