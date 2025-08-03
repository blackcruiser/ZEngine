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
    for (VulkanBuffer* buffer : _usedStagingBuffers)
    {
        delete buffer;
    }
    _usedStagingBuffers.clear();

    for (StagingBufferEntry& entry : _pendingStagingBufferEntries)
    {
        delete entry.buffer;
    }
    _pendingStagingBufferEntries.clear();

    for (VulkanBuffer* buffer : _freeStagingBuffers)
    {
        delete buffer;
    }
    _freeStagingBuffers.clear();

    ZE_CHECK(_usedStagingBuffers.empty());
    ZE_CHECK(_pendingStagingBufferEntries.empty());
}

VulkanBuffer* VulkanStagingBufferManager::AcquireStagingBuffer(uint32_t size)
{
    Recycle();

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

void VulkanStagingBufferManager::ReleaseStagingBuffer(VulkanBuffer* buffer, VulkanCommandBuffer* commandBuffer)
{
    if (commandBuffer == nullptr)
        _freeStagingBuffers.push_back(buffer);
    else
    {
        _pendingStagingBufferEntries.push_back({buffer, commandBuffer, commandBuffer->GetExecuteCount()});
    }

    _usedStagingBuffers.remove(buffer);
}

void VulkanStagingBufferManager::Recycle()
{
    for (auto iter = _pendingStagingBufferEntries.begin(); iter != _pendingStagingBufferEntries.end(); )
    {
        StagingBufferEntry& entry = *iter;
        if (entry.frameCount < entry.commandBuffer->GetExecuteCount())
        {
            _freeStagingBuffers.push_back(entry.buffer);
            iter = _pendingStagingBufferEntries.erase(iter);
        }
        else
        {
            iter ++;
        }
    }
}

} // namespace ZE
