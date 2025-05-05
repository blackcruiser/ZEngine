#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"
#include "VulkanDevice.h"


namespace ZE {

class VulkanShader : public VulkanDeviceChild
{
public:
    VulkanShader(VulkanDevice* device, const std::vector<char>& byteCode);
    ~VulkanShader();

    VkShaderModule GetRawShader();

private:
    VkShaderModule _shaderModule;
};

} // namespace ZE