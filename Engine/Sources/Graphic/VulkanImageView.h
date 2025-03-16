#pragma once

#include "CoreDefines.h"

#include <vulkan/vulkan.hpp>

namespace TE {

class VulkanDevice;
class VulkanImage;

class VulkanImageView
{
public:
    VulkanImageView(TPtr<VulkanDevice> device, TPtr<VulkanImage> image, VkFormat format = VkFormat::VK_FORMAT_UNDEFINED);
    ~VulkanImageView();

    VkImageView GetRawImageView();

private:
    VkFormat _format;

    VkImageView _vkImageView;

    TPtr<VulkanImage> _image;
    TPtr<VulkanDevice> _device;
};

} // namespace TE