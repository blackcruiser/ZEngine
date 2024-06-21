#include "VulkanImage.h"
#include "VulkanGPU.h"
#include "VulkanDevice.h"
#include "VulkanBuffer.h"
#include "VulkanCommandBuffer.h"
#include "VulkanCommandPool.h"

#include <stdexcept>


namespace ZE {

VulkanImage::VulkanImage(TPtr<VulkanDevice> device, const VkExtent3D& extent, VkFormat format, VkImageUsageFlags usageFlags)
    : _hasOwnship(true), _device(device), _extent(extent), _format(format), _vkImage(VK_NULL_HANDLE), _vkMemory(VK_NULL_HANDLE)
{
    VkDeviceSize size = _extent.width * _extent.height * 4;
    VkDevice vkDevice = _device->GetRawDevice();

    // Image
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent = extent;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usageFlags;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(vkDevice, &imageInfo, nullptr, &_vkImage) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create image!");
    }

    // Allocate memory
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(vkDevice, _vkImage, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = _device->GetGPU()->FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    if (vkAllocateMemory(vkDevice, &allocInfo, nullptr, &_vkMemory) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(vkDevice, _vkImage, _vkMemory, 0);
}

VulkanImage::VulkanImage(TPtr<VulkanDevice> device, VkImage vkImage, const VkExtent3D& extent, VkFormat format, VkImageUsageFlags usageFlags)
    : _hasOwnship(false), _device(device), _extent(extent), _format(format), _vkImage(vkImage), _vkMemory(VK_NULL_HANDLE)
{
}

VulkanImage::~VulkanImage()
{
    if (_hasOwnship)
    {
        VkDevice vkDevice = _device->GetRawDevice();

        if (_vkImage != VK_NULL_HANDLE)
            vkDestroyImage(vkDevice, _vkImage, nullptr);

        if (_vkMemory != VK_NULL_HANDLE)
            vkFreeMemory(vkDevice, _vkMemory, nullptr);
    }

    _vkMemory = VK_NULL_HANDLE;
    _vkImage = VK_NULL_HANDLE;
}

void VulkanImage::TransitionLayout(TPtr<VulkanCommandBuffer> commandBuffer, VkImageLayout oldLayout, VkImageLayout newLayout)
{
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = _vkImage;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else
    {
        throw std::invalid_argument("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(commandBuffer->GetRawCommandBuffer(), sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}


void VulkanImage::CopyFromBuffer(TPtr<VulkanCommandBuffer> commandBuffer, TPtr<VulkanBuffer> buffer, VkOffset3D offset, VkExtent3D extent)
{
    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = offset;
    region.imageExtent = extent;

    vkCmdCopyBufferToImage(commandBuffer->GetRawCommandBuffer(), buffer->GetRawBuffer(), _vkImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
}

void VulkanImage::TransferData(TPtr<VulkanCommandBuffer> commandBuffer, TPtr<VulkanBuffer> stagingBuffer, const void* data, uint32_t size)
{
    VkDeviceSize imageSize = size;

    void* mappedAddress = stagingBuffer->MapMemory(0, size);
    memcpy(mappedAddress, data, size);
    stagingBuffer->UnmapMemory();

    TransitionLayout(commandBuffer, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    CopyFromBuffer(commandBuffer, stagingBuffer, {0, 0, 0}, _extent);
    TransitionLayout(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

VkExtent3D VulkanImage::GetExtent()
{
    return _extent;
}

VkFormat VulkanImage::GetFormat()
{
    return _format;
}

TPtr<VulkanDevice> VulkanImage::GetDevice()
{
    return _device;
}

VkImage VulkanImage::GetRawImage()
{
    return _vkImage;
}

} // namespace ZE