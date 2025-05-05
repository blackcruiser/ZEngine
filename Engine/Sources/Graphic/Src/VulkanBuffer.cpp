#include "VulkanBuffer.h"
#include "VulkanContext.h"
#include "Misc/AssertionMacros.h"


namespace ZE {

VulkanBuffer::VulkanBuffer(VulkanDevice* device, uint32_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
    : VulkanDeviceChild(device), _buffer(VK_NULL_HANDLE),
    _memory(VK_NULL_HANDLE), _size(size), _usage(usage), _properties(properties)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult result = vkCreateBuffer(_device->GetRawDevice(), &bufferInfo, nullptr, &_buffer);
    CHECK_MSG(result == VkResult::VK_SUCCESS, "Create Buffer failed!");


    // Allocate memory
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(_device->GetRawDevice(), _buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(_device->GetRawPhysicalDevice(), memRequirements.memoryTypeBits, properties);

    result = vkAllocateMemory(_device->GetRawDevice(), &allocInfo, nullptr, &_memory);
    CHECK_MSG(result == VkResult::VK_SUCCESS, "Allocate memory failed!");

    result = vkBindBufferMemory(_device->GetRawDevice(), _buffer, _memory, 0);
    CHECK_MSG(result == VkResult::VK_SUCCESS, "Bind memory failed!");
}

VulkanBuffer::~VulkanBuffer()
{
    CHECK(_buffer != VK_NULL_HANDLE);
    vkDestroyBuffer(_device->GetRawDevice(), _buffer, nullptr);

    CHECK(_memory != VK_NULL_HANDLE);
    vkFreeMemory(_device->GetRawDevice(), _memory, nullptr);

    _memory = VK_NULL_HANDLE;
    _buffer = VK_NULL_HANDLE;
}

void* VulkanBuffer::MapMemory(VkDeviceSize offset, VkDeviceSize size)
{
    void* address = nullptr;
    vkMapMemory(_device->GetRawDevice(), _memory, offset, size, 0, &address);

    return address;
}

void VulkanBuffer::UnmapMemory()
{
    vkUnmapMemory(_device->GetRawDevice(), _memory);
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
    return _buffer;
}

VkDeviceMemory VulkanBuffer::GetRawMemory()
{
    return _memory;
}

} // namespace ZE