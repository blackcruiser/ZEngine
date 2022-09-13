#include "VulkanSampler.h"
#include "VulkanGPU.h"
#include "VulkanDevice.h"

#include <stdexcept>


namespace TE {

VulkanSampler::VulkanSampler(TPtr<VulkanDevice> device)
    : _device(device), _vkSampler(VK_NULL_HANDLE)
{
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(_device->GetGPU()->GetRawGPU(), &properties);

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

    if (vkCreateSampler(_device->GetRawDevice(), &samplerInfo, nullptr, &_vkSampler) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create texture sampler!");
    }
}

VulkanSampler::~VulkanSampler()
{
    if (_vkSampler != VK_NULL_HANDLE)
        vkDestroySampler(_device->GetRawDevice(), _vkSampler, nullptr);
}

VkSampler VulkanSampler::GetRawSampler()
{
    return _vkSampler;
}
} // namespace TE