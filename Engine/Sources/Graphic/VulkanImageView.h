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
    VulkanImageView(TPtr<VulkanImage> image, VkFormat format = VkFormat::VK_FORMAT_UNDEFINED);
    ~VulkanImageView();

    VkImageView GetRawImageView();

private:
    VkFormat _format;
    VkImageView _vkImageView;

    TPtr<VulkanImage> _image;
};

} // namespace ZE