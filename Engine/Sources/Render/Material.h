#include "CoreDefines.h"
#include "CoreTypes.h"
#include "Resource/MaterialResource.h"
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
class RenderingContext;
class RenderGraph;
class Mesh;

struct VulkanImageBindingInfo
{
    uint32_t bindingPoint;
    TPtr<VulkanImageView> vulkanImageView;
    TPtr<VulkanSampler> vulkanSampler;
};

class Pass
{
public:
    Pass(TPtr<PassResource> passResource);
    ~Pass();

    void BuildRenderResource(TPtr<RenderingContext> renderingContext, TPtr<RenderGraph> renderGraph);

private:
    void CreateGraphicTextures(TPtr<RenderingContext> renderingContext, TPtr<RenderGraph> renderGraph);
    void CreateGraphicBuffers(TPtr<RenderingContext> renderingContext, TPtr<RenderGraph> renderGraph);
    void CreateGraphicShaders();

    void CreateDescriptorSetLayout();
    void CreateDescriptorSet(TPtr<RenderingContext> renderingContext, TPtr<RenderGraph> renderGraph);
    void LinkDescriptorSet();
    void CreatePipelineLayout();

public:
    TPtr<VulkanDescriptorSet> GetDescriptorSet();

    TPtr<VulkanPipelineLayout> GetPipelineLayout();
    void ApplyPipelineState(RHIPipelineState& state);

    void UpdateUniformBuffer(TPtr<RenderingContext> renderingContext, TPtr<RenderGraph> renderGraph, const glm::mat4x4& mvp);

private:
    TPtrUnorderedMap<VkShaderStageFlagBits, VulkanShader> _shaders;
    std::unordered_map<VkShaderStageFlagBits, std::list<VulkanImageBindingInfo>> _textures;
    TPtr<VulkanBuffer> _uniformBuffer;
    TPtr<VulkanDescriptorSetLayout> _descriptorSetLayout;
    TPtr<VulkanDescriptorSet> _descriptorSet;
    TPtr<VulkanPipelineLayout> _pipelineLayout;

    RHIRasterizationState rasterizationState;
    RHIDepthStencilState depthStencilState;
    std::vector<RHIBlendState> blendStates;
    std::vector<RHIShaderState> shaderStates;

    TWeakPtr<PassResource> _owner;
};

class Material
{
public:
    Material(TPtr<MaterialResource> material);
    ~Material();

    void SetPass(EPassType passType, TPtr<Pass> pass);
    TPtr<Pass> GetPass(EPassType passType);

private:
    TPtrUnorderedMap<EPassType, Pass> _passMap;
    TWeakPtr<MaterialResource> _owner;
};

} // namespace ZE
