#include "VulkanBuffer.h"
#include "VulkanGPU.h"
#include "VulkanCommandBuffer.h"
#include "VulkanCommandPool.h"
#include "VulkanDevice.h"

#include <stdexcept>


namespace ZE {

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

    _vkMemory = VK_NULL_HANDLE;
    _vkBuffer = VK_NULL_HANDLE;
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

uint32_t VulkanBuffer::GetSize()
{
    return _size;
}

VkMemoryPropertyFlags VulkanBuffer::GetProperties()
{
    return _properties;
}

VkBuffer VulkanBuffer::GetRawBuffer()
{
    return _vkBuffer;
}

VkDeviceMemory VulkanBuffer::GetRawMemory()
{
    return _vkMemory;
}

} // namespace ZE