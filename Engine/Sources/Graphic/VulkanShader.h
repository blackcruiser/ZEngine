#pragma once

#include "CoreDefines.h"

#include <vulkan/vulkan.hpp>

namespace TE {

class Device;

class VulkanShader
{
public:
    VulkanShader(TPtr<Device> device, const std::vector<char>& byteCode);
    ~VulkanShader();

    VkShaderModule GetRawShader();

private:
    TPtr<Device> _device;

    VkShaderModule _vkShaderModule;
};

} // namespace TE