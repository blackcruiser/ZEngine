#include "VulkanBufferManager.h"
#include "VulkanBuffer.h"
#include "VulkanCommandBuffer.h"

#include <algorithm>
#include <iterator>


namespace ZE {

VulkanBufferManager::VulkanBufferManager(VulkanDevice* device)
    : VulkanDeviceChild(device)
{
}

VulkanBufferManager::~VulkanBufferManager()
{
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

    for (auto iter = _freeStagingBuffer.begin(); iter != _freeStagingBuffer.end(); iter++)
    {
        if ((*iter)->GetSize() >= size)
        {
            stagingBuffer = *iter;
            _freeStagingBuffer.erase(iter);
            break;
        }
    }

    if (stagingBuffer == nullptr)
    {
        stagingBuffer = new VulkanBuffer(_device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    }

    _usedStagingBuffer.push_back(stagingBuffer);

    return stagingBuffer;
}

void VulkanBufferManager::ReleaseStagingBuffer(VulkanBuffer* buffer, VulkanCommandBuffer* commandBuffer)
{
    if (commandBuffer == nullptr)
        _freeStagingBuffer.push_back(buffer);
    else
    {
        _pendingList.push_back({buffer, commandBuffer, commandBuffer->GetExecuteCount()});
    }

    _usedStagingBuffer.remove(buffer);
}

void VulkanBufferManager::Recycle()
{
    for (auto iter = _pendingList.begin(); iter != _pendingList.end(); )
    {
        StagingBufferEntry& entry = *iter;
        if (entry.frameCount < entry.commandBuffer->GetExecuteCount())
        {
            _freeStagingBuffer.push_back(entry.buffer);
            iter = _pendingList.erase(iter);
        }
        else
        {
            iter ++;
        }
    }
}

} // namespace ZE
