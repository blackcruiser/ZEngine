#include "VulkanImage.h"
#include "VulkanDevice.h"
#include "VulkanImageView.h"

#include <stdexcept>


namespace ZE {

VulkanImageView::VulkanImageView(TPtr<VulkanImage> image, VkFormat format, VkImageAspectFlagBits accessFlags)
    : _format(format), _accessFlags(accessFlags), _vkImageView(VK_NULL_HANDLE), _image(image)
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

    if (vkCreateImageView(image->GetDevice()->GetRawDevice(), &imageViewCreateInfo, nullptr, &_vkImageView) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create image views!");
    }
}

VulkanImageView::~VulkanImageView()
{
    if (_vkImageView != VK_NULL_HANDLE)
        vkDestroyImageView(_image->GetDevice()->GetRawDevice(), _vkImageView, nullptr);
}

TPtr<VulkanImage> VulkanImageView::GetImage()
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
    return _vkImageView;
}

} // namespace ZE