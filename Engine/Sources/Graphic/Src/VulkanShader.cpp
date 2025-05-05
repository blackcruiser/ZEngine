#include "VulkanShader.h"
#include "VulkanDevice.h"
#include "Misc/AssertionMacros.h"


namespace ZE {

VulkanShader::VulkanShader(VulkanDevice* device, const std::vector<char>& byteCode)
    : VulkanDeviceChild(device), _shaderModule(VK_NULL_HANDLE)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = byteCode.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(byteCode.data());

    VkResult result = vkCreateShaderModule(_device->GetRawDevice(), &createInfo, nullptr, &_shaderModule);
    CHECK_MSG(result == VkResult::VK_SUCCESS, "Failed to create ShaderModule!");
}

VulkanShader::~VulkanShader()
{
    CHECK(_shaderModule != VK_NULL_HANDLE);
    vkDestroyShaderModule(_device->GetRawDevice(), _shaderModule, nullptr);
}

VkShaderModule VulkanShader::GetRawShader()
{
    return _shaderModule;
}

} // namespace ZE