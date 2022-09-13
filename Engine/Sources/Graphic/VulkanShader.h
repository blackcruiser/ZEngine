#pragma once

#include "CoreDefines.h"

#include <vulkan/vulkan.h>

namespace TE {

class VulkanDevice;

class VulkanShader
{
public:
    VulkanShader(TPtr<VulkanDevice> device, const std::vector<char>& byteCode);
    ~VulkanShader();

    VkShaderModule GetRawShader();

private:
    TPtr<VulkanDevice> _device;

    VkShaderModule _vkShaderModule;
};

} // namespace TE