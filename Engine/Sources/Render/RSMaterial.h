#include "CoreDefines.h"

#include <Graphic/Device.h>

#include "Shader.h"

namespace TE {

class MaterialComponent;

class VulkanImage;
class VulkanShader;

class CommandPool;

struct VulkanImageBindingInfo
{
    uint32_t bindingPoint;
    TPtr<VulkanImage> vulkanImage;
};

class RSMaterial
{
public:
    RSMaterial(TPtr<Device> device, TPtr<MaterialComponent> material);
    ~RSMaterial();

    void CreateGraphicTextures(TPtr<Device> device, TPtr<CommandPool> commandPool);
    void CreateGraphicShaders(TPtr<Device> device);

    TPtr<VulkanShader> GetGraphicShader(EShaderStage stage);
    std::optional<std::reference_wrapper<std::list<VulkanImageBindingInfo>>> GetGraphicTexture(EShaderStage stage);

private:
    TPtrUnorderedMap<EShaderStage, VulkanShader> _shaders;
    std::unordered_map<EShaderStage, std::list<VulkanImageBindingInfo>> _textures;

    TPtr<Device> _device;
    TWeakPtr<MaterialComponent> _owner;

public:
    // Render Related
    VkPipeline vkPipeline;
    VkPipelineLayout vkPipelineLayout;

    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorSet descriptorSet;
    VkBuffer _uniformBuffer;
    VkDeviceMemory _uniformBufferMemory;
};

} // namespace TE