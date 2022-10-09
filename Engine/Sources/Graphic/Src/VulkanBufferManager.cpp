#include "VulkanBufferManager.h"
#include "VulkanDevice.h"
#include "VulkanBuffer.h"
#include "VulkanCommandBuffer.h"

#include <algorithm>
#include <iterator>


namespace ZE {

VulkanBufferManager::VulkanBufferManager(TPtr<VulkanDevice> device)
    : _device(device)
{
}

VulkanBufferManager::~VulkanBufferManager()
{
}

TPtr<VulkanBuffer> VulkanBufferManager::AcquireStagingBuffer(uint32_t size)
{
    TPtr<VulkanBuffer> stagingBuffer{nullptr};

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
        stagingBuffer = std::make_shared<VulkanBuffer>(_device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    }

    _usedStagingBuffer.push_back(stagingBuffer);

    return stagingBuffer;
}

void VulkanBufferManager::ReleaseStagingBuffer(TPtr<VulkanBuffer> buffer, TPtr<VulkanCommandBuffer> commandBuffer)
{
    if (commandBuffer == nullptr)
        _freeStagingBuffer.push_back(buffer);
    else
    {
        _pendingList.push_back({buffer, commandBuffer, commandBuffer->GetExecuteCount()});
    }

    _usedStagingBuffer.remove(buffer);
}

void VulkanBufferManager::Tick()
{
    auto removeFilterFunc = [](StagingBufferEntry& entry) {
        if (entry.weakCommandBuffer.expired())
            return true;
        else
        {
            TPtr<VulkanCommandBuffer> commandBuffer = entry.weakCommandBuffer.lock();
            if (entry.frameCount < commandBuffer->GetExecuteCount())
                return true;
            else
                return false;
        }
    };

    auto firstIter = std::find_if(_pendingList.begin(), _pendingList.end(), removeFilterFunc);
    if (firstIter != _pendingList.end())
    {
        _freeStagingBuffer.push_back(firstIter->buffer);

        for (auto iter = firstIter; ++iter != _pendingList.end();)
        {
            if (removeFilterFunc(*iter))
            {
                _freeStagingBuffer.push_back(iter->buffer);
            }
            else
            {
                *firstIter = std::move(*iter);
                firstIter++;
            }
        }

        _pendingList.erase(firstIter, _pendingList.end());
    }
}

} // namespace ZE
