#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"
#include "VulkanDevice.h"


namespace ZE {

class VulkanDevice;
class VulkanCommandBuffer;

class VulkanBuffer : public VulkanDeviceChild
{
public:
    VulkanBuffer(VulkanDevice* device, uint32_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
    ~VulkanBuffer();

    void* MapMemory(VkDeviceSize offset, VkDeviceSize size);
    void UnmapMemory();

    uint32_t GetSize();
    VkMemoryPropertyFlags GetProperties();

    VkBuffer GetRawBuffer();
    VkDeviceMemory GetRawMemory();

protected:
    VkBuffer _buffer;
    VkDeviceMemory _memory;
    uint32_t _size;
    VkBufferUsageFlags _usage;
    VkMemoryPropertyFlags _properties;
};

} // namespace ZE