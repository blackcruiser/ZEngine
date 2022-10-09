#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"


namespace ZE {

class VulkanDevice;
class VulkanBuffer;
class VulkanCommandBuffer;


struct StagingBufferEntry
{
    TPtr<VulkanBuffer> buffer;
    TWeakPtr<VulkanCommandBuffer> weakCommandBuffer;
    uint64_t frameCount;
};

class VulkanBufferManager
{
public:
    VulkanBufferManager(TPtr<VulkanDevice> device);
    ~VulkanBufferManager();

    TPtr<VulkanBuffer> AcquireStagingBuffer(uint32_t size);
    void ReleaseStagingBuffer(TPtr<VulkanBuffer> buffer, TPtr<VulkanCommandBuffer> commandBuffer = nullptr);

    void Tick();

private:
    TPtrList<VulkanBuffer> _usedStagingBuffer, _freeStagingBuffer;
    std::list<StagingBufferEntry> _pendingList;

    TPtr<VulkanDevice> _device;
};

} // namespace ZE