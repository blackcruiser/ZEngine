#include "VulkanImage.h"
#include "VulkanDevice.h"
#include "VulkanImageView.h"


namespace TE {

VulkanImageView::VulkanImageView(TPtr<VulkanDevice> device, TPtr<VulkanImage> image, VkFormat format)
    : _device(device), _image(image), _vkImageView(VK_NULL_HANDLE)
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
    imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = 1;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(_device->GetRawDevice(), &imageViewCreateInfo, nullptr, &_vkImageView) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create image views!");
    }
}

VulkanImageView::~VulkanImageView()
{
    if (_vkImageView != VK_NULL_HANDLE)
        vkDestroyImageView(_device->GetRawDevice(), _vkImageView, nullptr);
}

VkImageView VulkanImageView::GetRawImageView()
{
    return _vkImageView;
}

} // namespace TE