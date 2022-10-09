#include "CoreDefines.h"
#include "CoreTypes.h"
#include "Graphic/VulkanPipeline.h"

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

#include <list>
#include <optional>

namespace ZE {

class VulkanImage;
class VulkanSampler;
class VulkanImageView;
class VulkanShader;
class VulkanBuffer;
class VulkanDescriptorPool;
class VulkanDescriptorSet;
class VulkanDescriptorSetLayout;
class VulkanPipelineLayout;
class VulkanGraphicPipeline;
class VulkanCommandBuffer;
class MaterialResource;
class Mesh;

struct VulkanImageBindingInfo
{
    uint32_t bindingPoint;
    TPtr<VulkanImageView> vulkanImageView;
    TPtr<VulkanSampler> vulkanSampler;
};

class Material
{
public:
    Material(TPtr<MaterialResource> material);
    ~Material();

    void BuildRenderResource(TPtr<VulkanCommandBuffer> commandBuffer);

private:
    void CreateGraphicTextures(TPtr<VulkanCommandBuffer> commandBuffer);
    void CreateGraphicBuffers(TPtr<VulkanCommandBuffer> commandBuffer);
    void CreateGraphicShaders();

    void CreateDescriptorSetLayout();
    void CreateDescriptorSet();
    void LinkDescriptorSet();
    void CreatePipelineLayout();

public:
    TPtr<VulkanDescriptorSet> GetDescriptorSet();

    TPtr<VulkanPipelineLayout> GetPipelineLayout();
    void BuildPipelineDesc(VulkanGraphicPipelineDesc& desc);

    void UpdateUniformBuffer(TPtr<VulkanCommandBuffer> commandBuffer, const glm::mat4x4& mvp);

private:
    TPtrUnorderedMap<VkShaderStageFlagBits, VulkanShader> _shaders;
    std::unordered_map<VkShaderStageFlagBits, std::list<VulkanImageBindingInfo>> _textures;
    TPtr<VulkanBuffer> _uniformBuffer;
    TPtr<VulkanDescriptorSetLayout> _descriptorSetLayout;
    TPtr<VulkanDescriptorSet> _descriptorSet;
    TPtr<VulkanPipelineLayout> _pipelineLayout;
    VulkanGraphicPipelineDesc _pipelineDesc;

    TWeakPtr<MaterialResource> _owner;
};

} // namespace ZE
