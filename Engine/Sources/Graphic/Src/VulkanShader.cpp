#include "VulkanShader.h"
#include "CommandPool.h"
#include "Device.h"

namespace TE {

VulkanShader::VulkanShader(TPtr<Device> device, const std::vector<char>& byteCode)
    : _device(device), _vkShaderModule(VK_NULL_HANDLE)
{
    _vkShaderModule = _device->CreateShaderModule(byteCode);
}

VulkanShader::~VulkanShader()
{
    if (_vkShaderModule != VK_NULL_HANDLE)
        _device->DestroyShaderModule(_vkShaderModule);
}

VkShaderModule VulkanShader::GetRawShader()
{
    return _vkShaderModule;
}

} // namespace TE