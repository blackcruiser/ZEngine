#include "VulkanImage.h"
#include "VulkanContext.h"
#include "VulkanCommandBuffer.h"
#include "Misc/AssertionMacros.h"


namespace ZE {

VulkanImage::VulkanImage(VulkanDevice* device, const VkExtent3D& extent, VkFormat format, VkImageUsageFlags usageFlags)
    : VulkanDeviceChild(device), _image(VK_NULL_HANDLE), _memory(VK_NULL_HANDLE)
    , _hasOwnship(true), _extent(extent), _format(format), _layout(VK_IMAGE_LAYOUT_UNDEFINED)
{
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

    VkResult result = vkCreateImage(_device->GetRawDevice(), &imageInfo, nullptr, &_image);
    CHECK_MSG(result == VkResult::VK_SUCCESS, "Failed to create image!")

    // Allocate memory
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(_device->GetRawDevice(), _image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(_device->GetRawPhysicalDevice(), memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    result = vkAllocateMemory(_device->GetRawDevice(), &allocInfo, nullptr, &_memory);
    CHECK_MSG(result == VkResult::VK_SUCCESS, "Failed to allocate image memory!")

    vkBindImageMemory(_device->GetRawDevice(), _image, _memory, 0);
}

VulkanImage::VulkanImage(VulkanDevice* device, VkImage vkImage, const VkExtent3D& extent, VkFormat format, VkImageUsageFlags usageFlags)
    : VulkanDeviceChild(device), _image(vkImage), _memory(VK_NULL_HANDLE), _hasOwnship(false), _extent(extent), _format(format), _layout(VK_IMAGE_LAYOUT_UNDEFINED)
{
}

VulkanImage::~VulkanImage()
{
    if (_hasOwnship)
    {
        VkDevice rawDevice = _device->GetRawDevice();

        CHECK(_image != VK_NULL_HANDLE);
        vkDestroyImage(rawDevice, _image, nullptr);

        CHECK(_memory != VK_NULL_HANDLE);
        vkFreeMemory(rawDevice, _memory, nullptr);
    }

    _memory = VK_NULL_HANDLE;
    _image = VK_NULL_HANDLE;
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

VkImage VulkanImage::GetRawImage()
{
    return _image;
}

} // namespace ZE