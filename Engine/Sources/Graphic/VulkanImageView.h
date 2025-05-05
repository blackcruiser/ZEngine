#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"
#include "VulkanDevice.h"


namespace ZE {

class VulkanDevice;
class VulkanImage;

class VulkanImageView
{
public:
    VulkanImageView(VulkanImage* image, VkFormat format = VkFormat::VK_FORMAT_UNDEFINED, VkImageAspectFlagBits accessFlags = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT);
    ~VulkanImageView();

    VulkanImage* GetImage();

    VkFormat GetFormat();
    VkExtent3D GetExtent();

    VkImageView GetRawImageView();

private:
    VkImageView _imageView;
    VkFormat _format;
    VkImageAspectFlagBits _accessFlags;

    VulkanImage* _image;
};

} // namespace ZE