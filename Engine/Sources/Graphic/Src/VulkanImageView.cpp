#include "VulkanImageView.h"
#include "VulkanImage.h"
#include "Misc/AssertionMacros.h"


namespace ZE {

VulkanImageView::VulkanImageView(VulkanImage* image, VkFormat format, VkImageAspectFlagBits accessFlags)
    : _imageView(VK_NULL_HANDLE), _format(format), _accessFlags(accessFlags), _image(image)
{
    VkImageViewCreateInfo imageViewCreateInfo{};
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.image = image->GetRawImage();
    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.format = format == VK_FORMAT_UNDEFINED ? image->GetFormat() : format;
    imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.subresourceRange.aspectMask = accessFlags;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = 1;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = 1;

    VkResult result = vkCreateImageView(image->GetDevice()->GetRawDevice(), &imageViewCreateInfo, nullptr, &_imageView);
    CHECK_MSG(result == VkResult::VK_SUCCESS, "Failed to create ImageView!");
}

VulkanImageView::~VulkanImageView()
{
    CHECK(_imageView != VK_NULL_HANDLE);
    vkDestroyImageView(_image->GetDevice()->GetRawDevice(), _imageView, nullptr);
}

VulkanImage* VulkanImageView::GetImage()
{
    return _image;
}

VkFormat VulkanImageView::GetFormat()
{
    return _format == VK_FORMAT_UNDEFINED ? _image->GetFormat() : _format;
}

VkExtent3D VulkanImageView::GetExtent()
{
    return _image->GetExtent();
}

VkImageView VulkanImageView::GetRawImageView()
{
    return _imageView;
}

} // namespace ZE