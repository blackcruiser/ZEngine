#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"

#include <vulkan/vulkan.h>

namespace ZE {

class VulkanDevice;
class VulkanImage;

class VulkanImageView
{
public:
    VulkanImageView(TPtr<VulkanImage> image, VkFormat format = VkFormat::VK_FORMAT_UNDEFINED, VkImageAspectFlagBits accessFlags = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT);
    ~VulkanImageView();

    VkFormat GetFormat();
    VkExtent3D GetExtent();

    VkImageView GetRawImageView();

private:
    VkFormat _format;
    VkImageAspectFlagBits _accessFlags;
    VkImageView _vkImageView;

    TPtr<VulkanImage> _image;
};

} // namespace ZE