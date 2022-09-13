#include "VulkanBuffer.h"
#include "VulkanGPU.h"
#include "VulkanCommandBuffer.h"
#include "VulkanCommandPool.h"
#include "VulkanDevice.h"

#include <stdexcept>


namespace TE {

VulkanBuffer::VulkanBuffer(TPtr<VulkanDevice> device, uint32_t size, VkBufferUsageFlags usage,
                           VkMemoryPropertyFlags properties)
    : _device(device), _size(size), _usage(usage), _properties(properties), _vkBuffer(VK_NULL_HANDLE),
      _vkMemory(VK_NULL_HANDLE)
{
    VkDevice vkDevice = _device->GetRawDevice();

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(vkDevice, &bufferInfo, nullptr, &_vkBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create buffer!");
    }


    // Allocate memory
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(vkDevice, _vkBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = _device->GetGPU()->FindMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(vkDevice, &allocInfo, nullptr, &_vkMemory) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    if (vkBindBufferMemory(vkDevice, _vkBuffer, _vkMemory, 0) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to bind buffer memory!");
    }
}

VulkanBuffer::~VulkanBuffer()
{
    VkDevice vkDevice = _device->GetRawDevice();

    if (_vkBuffer != VK_NULL_HANDLE)
        vkDestroyBuffer(vkDevice, _vkBuffer, nullptr);

    if (_vkMemory != VK_NULL_HANDLE)
        vkFreeMemory(vkDevice, _vkMemory, nullptr);
}

void VulkanBuffer::CopyFromBuffer(TPtr<VulkanCommandPool> commandPool, TPtr<VulkanBuffer> otherBuffer,
                VkDeviceSize size)
{
    TPtr<VulkanCommandBuffer> commandBuffer = std::make_shared<VulkanCommandBuffer>(commandPool);
    VkCommandBuffer vkCommandBuffer = commandBuffer->GetRawCommandBuffer();

    commandBuffer->Begin();

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(vkCommandBuffer, otherBuffer->GetRawBuffer(), _vkBuffer, 1, &copyRegion);

    commandBuffer->End();

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &vkCommandBuffer;

    vkQueueSubmit(_device->GetGraphicQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(_device->GetGraphicQueue());
}

void VulkanBuffer::TransferData(TPtr<VulkanCommandPool> commandPool, const void* data, uint32_t size)
{
    if (_properties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
    {
        void* mappedAddress = nullptr;
        vkMapMemory(_device->GetRawDevice(), _vkMemory, 0, size, 0, &mappedAddress);
        memcpy(mappedAddress, data, size);
        vkUnmapMemory(_device->GetRawDevice(), _vkMemory);
    }
    else if (_properties & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
    {
        TPtr<VulkanBuffer> stagingBuffer = std::make_shared<VulkanBuffer>(_device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        void* mappedAddress = stagingBuffer->MapMemory(0, size);
        memcpy(mappedAddress, data, size);
        stagingBuffer->UnmapMemory();

        CopyFromBuffer(commandPool, stagingBuffer, size);
    }
}

void* VulkanBuffer::MapMemory(VkDeviceSize offset, VkDeviceSize size)
{
    void* address = nullptr;
    vkMapMemory(_device->GetRawDevice(), _vkMemory, offset, size, 0, &address);

    return address;
}

void VulkanBuffer::UnmapMemory()
{
    vkUnmapMemory(_device->GetRawDevice(), _vkMemory);
}

VkBuffer VulkanBuffer::GetRawBuffer()
{
    return _vkBuffer;
}

} // namespace TE