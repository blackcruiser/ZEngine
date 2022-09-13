#pragma once

#include "CoreDefines.h"

#include <vulkan/vulkan.h>

namespace TE {

class VulkanDevice;
class VulkanCommandPool;
class VulkanBuffer;

class VulkanImage
{
public:
    VulkanImage(TPtr<VulkanDevice> device, const VkExtent3D& extent, VkFormat format);
    VulkanImage(TPtr<VulkanDevice> device, VkImage vkImage, const VkExtent3D& extent, VkFormat format);
    ~VulkanImage();

    void TransitionLayout(TPtr<VulkanCommandPool> commandPool, VkImageLayout oldLayout, VkImageLayout newLayout);
    void CopyFromBuffer(TPtr<VulkanCommandPool> commandPool, TPtr<VulkanBuffer> buffer, VkOffset3D offset, VkExtent3D extent);
    void TransferData(TPtr<VulkanCommandPool> commandPool, const void* data, uint32_t size);

    VkFormat GetFormat();

    VkImage GetRawImage();

private:
    bool _hasOwnship;
    VkExtent3D _extent;
    VkFormat _format;

    VkImage _vkImage;
    VkDeviceMemory _vkMemory;

    TPtr<VulkanDevice> _device;
};

} // namespace TE