#include "CoreDefines.h"
#include "CoreTypes.h"
#include "Resource/MaterialResource.h"
#include "Graphic/VulkanPipeline.h"
#include "RenderResource.h"

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

#include <list>
#include <optional>


namespace ZE {

class VulkanSampler;
class VulkanImage;
class VulkanShader;
class VulkanBuffer;
class VulkanDescriptorSet;
class VulkanDescriptorSetLayout;
class VulkanPipelineLayout;
class RenderGraph;

struct VulkanImageBindingInfo
{
    uint32_t bindingPoint;
    TPtr<VulkanImage> vulkanImage;
    TPtr<VulkanSampler> vulkanSampler;
};

class Pass : public RenderResource
{
public:
    Pass(PassResource* passResource);
    ~Pass();

    virtual void InitGraphic(RenderGraph* renderGraph) override;
    virtual void CleanupGraphic() override;

private:
    void CreateGraphicTextures(RenderGraph* renderGraph);
    void CreateGraphicBuffers(RenderGraph* renderGraph);
    void CreateGraphicShaders(RenderGraph* renderGraph);

    void CreateDescriptorSetLayout(RenderGraph* renderGraph);
    void CreateDescriptorSet(RenderGraph* renderGraph);
    void LinkDescriptorSet(RenderGraph* renderGraph);
    void CreatePipelineLayout(RenderGraph* renderGraph);

public:
    VulkanDescriptorSet* GetDescriptorSet();

    VulkanPipelineLayout* GetPipelineLayout();
    void ApplyPipelineState(RHIPipelineState& state);

    void UpdateUniformBuffer(RenderGraph* renderGraph, const glm::mat4x4& mvp);

private:
    std::unordered_map<VkShaderStageFlagBits, VulkanShader*> _shaders;
    std::unordered_map<VkShaderStageFlagBits, std::list<VulkanImageBindingInfo>> _textures;
    TPtr<VulkanBuffer> _uniformBuffer;
    VulkanDescriptorSetLayout* _descriptorSetLayout;
    VulkanDescriptorSet* _descriptorSet;
    VulkanPipelineLayout* _pipelineLayout;

    RHIRasterizationState rasterizationState;
    RHIDepthStencilState depthStencilState;
    std::vector<RHIBlendState> blendStates;
    std::vector<RHIShaderState> shaderStates;

    PassResource* _owner;
};

class Material : public RenderResource
{
public:
    Material(MaterialResource* material);
    ~Material();

    void SetPass(EPassType passType, TPtr<Pass> pass);
    TPtr<Pass> GetPass(EPassType passType);

private:
    TPtrUnorderedMap<EPassType, Pass> _passMap;
    MaterialResource* _owner;
};

} // namespace ZE
