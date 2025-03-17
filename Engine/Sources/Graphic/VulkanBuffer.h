#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"

#include <vulkan/vulkan.h>

namespace ZE {

class VulkanDevice;
class VulkanCommandPool;

class VulkanBuffer
{
public:
    VulkanBuffer(TPtr<VulkanDevice> device, uint32_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
    ~VulkanBuffer();

    void CopyFromBuffer(TPtr<VulkanCommandPool> commandPool, TPtr<VulkanBuffer> otherBuffer, VkDeviceSize size);
    void TransferData(TPtr<VulkanCommandPool> commandPool, const void* data, uint32_t size);

    void* MapMemory(VkDeviceSize offset, VkDeviceSize size);
    void UnmapMemory();

    VkBuffer GetRawBuffer();
    VkDeviceMemory GetRawMemory();

private:
    uint32_t _size;
    VkBufferUsageFlags _usage;
    VkMemoryPropertyFlags _properties;
    VkBuffer _vkBuffer;
    VkDeviceMemory _vkMemory;

    TPtr<VulkanDevice> _device;
};

} // namespace ZE