#include "VulkanImage.h"
#include "CommandBuffer.h"
#include "CommandPool.h"
#include "Device.h"

namespace TE {

VulkanImage::VulkanImage(TPtr<Device> device, uint32_t width, uint32_t height, VkFormat format)
    : _width(width), _height(height), _device(device), _vkImage(VK_NULL_HANDLE), _vkMemory(VK_NULL_HANDLE),
      _vkSampler(VK_NULL_HANDLE)
{

    VkDeviceSize size = width * height * 4;
    _vkImage = device->CreateImage(width, height, format, VK_IMAGE_TILING_OPTIMAL,
                                   VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
    _vkMemory = device->AllocateAndBindImageMemory(_vkImage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    _vkImageView = device->CreateImageView(_vkImage, VK_FORMAT_R8G8B8A8_SRGB);
    _vkSampler = device->CreateTextureSampler();
}

VulkanImage::~VulkanImage()
{
    if (_vkImageView != VK_NULL_HANDLE)
        _device->DestroyImageView(_vkImageView);

    if (_vkImage != VK_NULL_HANDLE)
        _device->DestroyImage(_vkImage);

    if (_vkSampler != VK_NULL_HANDLE)
        _device->DestroyTextureSampler(_vkSampler);

    if (_vkMemory != VK_NULL_HANDLE)
        _device->FreeMemory(_vkMemory);
}

void VulkanImage::TransitionLayout(TPtr<CommandPool> commandPool, VkImageLayout oldLayout, VkImageLayout newLayout)
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


    CommandBuffer* commandBuffer = commandPool->CreateCommandBuffer(commandPool);
    VkCommandBuffer vkCommandBuffer = commandBuffer->GetRawCommandBuffer();

    commandBuffer->Begin();
    vkCmdPipelineBarrier(vkCommandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    commandBuffer->End();

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &vkCommandBuffer;

    vkQueueSubmit(_device->GetGraphicQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(_device->GetGraphicQueue());

    commandPool->DestroyCommandBuffer(commandBuffer);
}


void VulkanImage::CopyBufferToImage(TPtr<CommandPool> commandPool, VkBuffer buffer, VkOffset3D offset,
                                    VkExtent3D extent)
{
    CommandBuffer* commandBuffer = commandPool->CreateCommandBuffer(commandPool);
    VkCommandBuffer vkCommandBuffer = commandBuffer->GetRawCommandBuffer();

    commandBuffer->Begin();

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

    vkCmdCopyBufferToImage(vkCommandBuffer, buffer, _vkImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    commandBuffer->End();

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &vkCommandBuffer;

    vkQueueSubmit(_device->GetGraphicQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(_device->GetGraphicQueue());

    commandPool->DestroyCommandBuffer(commandBuffer);
}

void VulkanImage::UploadData(TPtr<CommandPool> commandPool, const void* data, uint32_t size)
{
    VkDeviceSize imageSize = size;
    VkBuffer textureStagingBuffer;
    VkDeviceMemory textureStagingBufferMemory;
    textureStagingBuffer = _device->CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    textureStagingBufferMemory = _device->AllocateAndBindBufferMemory(
        textureStagingBuffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);


    void* mappedAddress;
    vkMapMemory(_device->GetRawDevice(), textureStagingBufferMemory, 0, imageSize, 0, &mappedAddress);
    memcpy(mappedAddress, data, size);
    vkUnmapMemory(_device->GetRawDevice(), textureStagingBufferMemory);


    TransitionLayout(commandPool, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    CopyBufferToImage(commandPool, textureStagingBuffer, {0, 0, 0}, {_width, _height, 1});
    TransitionLayout(commandPool, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    _device->FreeMemory(textureStagingBufferMemory);
    _device->DestroyBuffer(textureStagingBuffer);
}

VkImage VulkanImage::GetRawImage()
{
    return _vkImage;
}

VkImageView VulkanImage::GetRawImageView()
{
    return _vkImageView;
}

VkSampler VulkanImage::GetRawSampler()
{
    return _vkSampler;
}
} // namespace TE