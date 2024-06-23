#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"

#include <vulkan/vulkan.h>

namespace ZE {

class VulkanDevice;
class VulkanCommandBuffer;
class VulkanBuffer;

class VulkanImage
{
public:
    VulkanImage(TPtr<VulkanDevice> device, const VkExtent3D& extent, VkFormat format, VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
    VulkanImage(TPtr<VulkanDevice> device, VkImage vkImage, const VkExtent3D& extent, VkFormat format, VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
    ~VulkanImage();

    void TransitionLayout(TPtr<VulkanCommandBuffer> commandBuffer, VkImageLayout oldLayout, VkImageLayout newLayout);
    void CopyFromBuffer(TPtr<VulkanCommandBuffer> commandBuffer, TPtr<VulkanBuffer> buffer, VkOffset3D offset, VkExtent3D extent);
    void TransferData(TPtr<VulkanCommandBuffer> commandBuffer, TPtr<VulkanBuffer> stagingBuffer, const void* data, uint32_t size);

    void SetLayout(VkImageLayout layout);
    VkImageLayout GetLayout();

    VkExtent3D GetExtent();
    VkFormat GetFormat();

    TPtr<VulkanDevice> GetDevice();

    VkImage GetRawImage();

private:
    bool _hasOwnship;
    VkExtent3D _extent;
    VkFormat _format;
    VkImageLayout _layout;

    VkImage _vkImage;
    VkDeviceMemory _vkMemory;

    TPtr<VulkanDevice> _device;
};

} // namespace ZE