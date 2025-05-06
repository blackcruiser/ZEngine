#include "VulkanBufferManager.h"
#include "VulkanBuffer.h"
#include "VulkanCommandBuffer.h"
#include "Misc/AssertionMacros.h"

#include <algorithm>
#include <iterator>


namespace ZE {

VulkanBufferManager::VulkanBufferManager(VulkanDevice* device)
    : VulkanDeviceChild(device)
{
}

VulkanBufferManager::~VulkanBufferManager()
{
    Recycle();

    for (VulkanBuffer* buffer : _freeStagingBuffers)
    {
        delete buffer;
    }

    CHECK(_usedStagingBuffers.empty());
    CHECK(_pendingStagingBufferEntries.empty());
}

VulkanBuffer* VulkanBufferManager::AcquireBuffer(uint32_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
{
    return  new VulkanBuffer(_device, size, usage, properties);
}

void VulkanBufferManager::ReleaseBuffer(VulkanBuffer* buffer, VulkanCommandBuffer* commandBuffer)
{
    delete buffer;
}

VulkanBuffer* VulkanBufferManager::AcquireStagingBuffer(uint32_t size)
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

void VulkanBufferManager::ReleaseStagingBuffer(VulkanBuffer* buffer, VulkanCommandBuffer* commandBuffer)
{
    if (commandBuffer == nullptr)
        _freeStagingBuffers.push_back(buffer);
    else
    {
        _pendingStagingBufferEntries.push_back({buffer, commandBuffer, commandBuffer->GetExecuteCount()});
    }

    _usedStagingBuffers.remove(buffer);
}

void VulkanBufferManager::Recycle()
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
