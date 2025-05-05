#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"
#include "VulkanDevice.h"


namespace ZE {

class VulkanBuffer;
class VulkanCommandBuffer;


struct StagingBufferEntry
{
    VulkanBuffer* buffer;
    VulkanCommandBuffer* commandBuffer;
    uint64_t frameCount;
};

class VulkanBufferManager : public VulkanDeviceChild
{
public:
    VulkanBufferManager(VulkanDevice* device);
    ~VulkanBufferManager();

    VulkanBuffer* AcquireBuffer(uint32_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
    void ReleaseBuffer(VulkanBuffer* buffer, VulkanCommandBuffer* commandBuffer = nullptr);

    VulkanBuffer* AcquireStagingBuffer(uint32_t size);
    void ReleaseStagingBuffer(VulkanBuffer* buffer, VulkanCommandBuffer* commandBuffer = nullptr);

private:
    void Recycle();

private:
    std::list<VulkanBuffer*> _usedStagingBuffer, _freeStagingBuffer;
    std::list<StagingBufferEntry> _pendingList;
};

} // namespace ZE