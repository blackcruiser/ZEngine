#include "VulkanShader.h"
#include "VulkanDevice.h"

namespace TE {

VulkanShader::VulkanShader(TPtr<VulkanDevice> device, const std::vector<char>& byteCode)
    : _device(device), _vkShaderModule(VK_NULL_HANDLE)
{

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = byteCode.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(byteCode.data());

    if (vkCreateShaderModule(_device->GetRawDevice(), &createInfo, nullptr, &_vkShaderModule) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create shader module!");
    }
}

VulkanShader::~VulkanShader()
{
    if (_vkShaderModule != VK_NULL_HANDLE)
        vkDestroyShaderModule(_device->GetRawDevice(), _vkShaderModule, nullptr);
}

VkShaderModule VulkanShader::GetRawShader()
{
    return _vkShaderModule;
}

} // namespace TE