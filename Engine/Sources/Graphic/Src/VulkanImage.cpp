#include "VulkanImage.h"
#include "VulkanGPU.h"
#include "VulkanDevice.h"
#include "VulkanBuffer.h"
#include "VulkanCommandBuffer.h"
#include "VulkanCommandPool.h"

#include <stdexcept>


namespace ZE {

VulkanImage::VulkanImage(TPtr<VulkanDevice> device, const VkExtent3D& extent, VkFormat format, VkImageUsageFlags usageFlags)
    : _hasOwnship(true), _device(device), _extent(extent), _format(format), _layout(VK_IMAGE_LAYOUT_UNDEFINED), _vkImage(VK_NULL_HANDLE), _vkMemory(VK_NULL_HANDLE)
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
    : _hasOwnship(false), _device(device), _extent(extent), _format(format), _layout(VK_IMAGE_LAYOUT_UNDEFINED), _vkImage(vkImage), _vkMemory(VK_NULL_HANDLE)
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

void VulkanImage::SetLayout(VkImageLayout layout)
{
    _layout = layout;
}

VkImageLayout VulkanImage::GetLayout()
{
    return _layout;
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