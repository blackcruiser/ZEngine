#include "VulkanSampler.h"
#include "Misc/AssertionMacros.h"


namespace ZE {

VulkanSampler::VulkanSampler(VulkanDevice* device)
    : VulkanDeviceChild(device), _sampler(VK_NULL_HANDLE)
{
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(_device->GetRawPhysicalDevice(), &properties);

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    VkResult result = vkCreateSampler(_device->GetRawDevice(), &samplerInfo, nullptr, &_sampler);
    CHECK_MSG(result == VkResult::VK_SUCCESS, "Failed to create Sampler!")
}

VulkanSampler::~VulkanSampler()
{
    CHECK(_sampler != VK_NULL_HANDLE);
    vkDestroySampler(_device->GetRawDevice(), _sampler, nullptr);
}

VkSampler VulkanSampler::GetRawSampler()
{
    return _sampler;
}
} // namespace ZE

