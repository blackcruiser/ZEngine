#pragma once

#include "CoreDefines.h"

#include <vulkan/vulkan.hpp>

namespace TE {

class Device;
class CommandPool;

class VulkanImage
{
public:
    VulkanImage(TPtr<Device> device, uint32_t width, uint32_t height, VkFormat format);
    ~VulkanImage();

    void TransitionLayout(TPtr<CommandPool> commandPool, VkImageLayout oldLayout, VkImageLayout newLayout);
    void CopyBufferToImage(TPtr<CommandPool> commandPool, VkBuffer buffer, VkOffset3D offset, VkExtent3D extent);
    void UploadData(TPtr<CommandPool> commandPool, const void* data, uint32_t size);

    VkImage GetRawImage();
    VkImageView GetRawImageView();
    VkSampler GetRawSampler();

private:
    uint32_t _width, _height;
    VkFormat _format;

    VkImage _vkImage;
    VkDeviceMemory _vkMemory;
    VkImageView _vkImageView;
    VkSampler _vkSampler;

    TPtr<Device> _device;
};

} // namespace TE