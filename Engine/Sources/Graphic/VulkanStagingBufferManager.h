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

class VulkanStagingBufferManager : public VulkanDeviceChild
{
public:
    VulkanStagingBufferManager(VulkanDevice* device);
    ~VulkanStagingBufferManager();

    VulkanBuffer* AcquireStagingBuffer(uint32_t size);
    void ReleaseStagingBuffer(VulkanBuffer* buffer, VulkanCommandBuffer* commandBuffer = nullptr);

private:
    void Recycle();

private:
    std::list<VulkanBuffer*> _usedStagingBuffers, _freeStagingBuffers;
    std::list<StagingBufferEntry> _pendingStagingBufferEntries;
};

} // namespace ZE