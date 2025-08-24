#include "VulkanStagingBufferManager.h"
#include "VulkanSynchronizer.h"
#include "VulkanBuffer.h"
#include "VulkanCommandBuffer.h"
#include "Render/RenderSystem.h"
#include "Debug/AssertionMacros.h"

#include <algorithm>
#include <iterator>


namespace ZE {

VulkanStagingBufferManager::VulkanStagingBufferManager(VulkanDevice* device)
    : VulkanDeviceChild(device)
{
}

VulkanStagingBufferManager::~VulkanStagingBufferManager()
{
    for (VulkanBuffer* buffer : _freeStagingBuffers)
    {
        delete buffer;
    }
    _freeStagingBuffers.clear();

    ZE_CHECK(_usedStagingBuffers.empty());
    ZE_CHECK(_pendingStagingBuffers.empty());
}

VulkanBuffer* VulkanStagingBufferManager::AcquireStagingBuffer(uint32_t size)
{
    VulkanBuffer* stagingBuffer = nullptr;

    for (auto iter = _freeStagingBuffers.begin(); iter != _freeStagingBuffers.end(); iter++)
    {
        if ((*iter)->GetSize() >= size)
        {
            stagingBuffer = *iter;
            _freeStagingBuffers.erase(iter);
            break;
        }
    }

    if (stagingBuffer == nullptr)
    {
        stagingBuffer = new VulkanBuffer(_device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    }

    _usedStagingBuffers.push_back(stagingBuffer);

    return stagingBuffer;
}

void VulkanStagingBufferManager::ReleaseStagingBuffer(VulkanBuffer* buffer)
{
    _pendingStagingBuffers.push_back(buffer);

    _usedStagingBuffers.remove(buffer);
}

void VulkanStagingBufferManager::Recycle(uint32 frameNumber)
{
    for (auto iter = _pendingStagingBuffers.begin(); iter != _pendingStagingBuffers.end(); )
    {
        VulkanBuffer* buffer = *iter;
        if (buffer->GetUsedFrameNumber() <= frameNumber)
        {
            _freeStagingBuffers.push_back(buffer);
            iter = _pendingStagingBuffers.erase(iter);
        }
        else
        {
            iter ++;
        }
    }
}

} // namespace ZE
