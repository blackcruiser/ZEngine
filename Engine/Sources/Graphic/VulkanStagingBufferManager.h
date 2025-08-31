#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"
#include "VulkanDevice.h"


namespace ZE {

class VulkanBuffer;
class VulkanCommandBuffer;


class VulkanStagingBufferManager : public VulkanDeviceChild
{
public:
    VulkanStagingBufferManager(VulkanDevice* device);
    ~VulkanStagingBufferManager();

    VulkanBuffer* AcquireStagingBuffer(uint32_t size);
    void ReleaseStagingBuffer(VulkanBuffer* buffer);

    void Recycle(uint32 executeCounter);

private:
    std::list<VulkanBuffer*> _usedStagingBuffers, _freeStagingBuffers, _pendingStagingBuffers;
};

} // namespace ZE