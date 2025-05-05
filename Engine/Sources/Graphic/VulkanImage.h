#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"
#include "VulkanDevice.h"


namespace ZE {

class VulkanDevice;
class VulkanCommandBuffer;
class VulkanBuffer;

class VulkanImage : public VulkanDeviceChild
{
public:
    VulkanImage(VulkanDevice* device, const VkExtent3D& extent, VkFormat format, VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
    VulkanImage(VulkanDevice* device, VkImage vkImage, const VkExtent3D& extent, VkFormat format, VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
    ~VulkanImage();

    void TransitionLayout(VulkanCommandBuffer* commandBuffer, VkImageLayout oldLayout, VkImageLayout newLayout);
    void CopyFromBuffer(VulkanCommandBuffer* commandBuffer, VulkanBuffer* buffer, VkOffset3D offset, VkExtent3D extent);
    void TransferData(VulkanCommandBuffer* commandBuffer, VulkanBuffer* stagingBuffer, const void* data, uint32_t size);

    void SetLayout(VkImageLayout layout);
    VkImageLayout GetLayout();

    VkExtent3D GetExtent();
    VkFormat GetFormat();

    VkImage GetRawImage();

private:
    VkImage _image;
    VkDeviceMemory _memory;
    bool _hasOwnship;
    VkExtent3D _extent;
    VkFormat _format;
    VkImageLayout _layout;
};

} // namespace ZE