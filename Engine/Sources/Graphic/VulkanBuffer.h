#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"

#include <vulkan/vulkan.h>


namespace ZE {

class VulkanDevice;
class VulkanCommandBuffer;

class VulkanBuffer
{
public:
    VulkanBuffer(TPtr<VulkanDevice> device, uint32_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
    ~VulkanBuffer();

    void* MapMemory(VkDeviceSize offset, VkDeviceSize size);
    void UnmapMemory();

    uint32_t GetSize();
    VkMemoryPropertyFlags GetProperties();

    VkBuffer GetRawBuffer();
    VkDeviceMemory GetRawMemory();

protected:
    uint32_t _size;
    VkBufferUsageFlags _usage;
    VkMemoryPropertyFlags _properties;
    VkBuffer _vkBuffer;
    VkDeviceMemory _vkMemory;

    TPtr<VulkanDevice> _device;
};

} // namespace ZE