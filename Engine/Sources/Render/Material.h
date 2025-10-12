#include "CoreDefines.h"
#include "CoreTypes.h"
#include "Resource/MaterialResource.h"
#include "Graphic/VulkanPipeline.h"
#include "RenderResource.h"

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

#include <list>
#include <unordered_map>
#include <optional>


namespace ZE {

class VulkanShader;
class VulkanBuffer;
class VulkanDescriptorSet;
class VulkanDescriptorSetLayout;
class VulkanPipelineLayout;
class Texture;
class RenderGraph;

struct TextureBinding
{
    Texture* texture;
    VkShaderStageFlags flagBits;
};

class Pass : public RenderResource
{
public:
    Pass(PassResource* passResource);
    ~Pass();

    virtual void InitGraphic(RenderGraph* renderGraph) override;
    virtual void CleanupGraphic() override;

private:
    void CreateGraphicShaders(RenderGraph* renderGraph);

    void CreateDescriptorSetLayout(RenderGraph* renderGraph);
    void CreateDescriptorSet(RenderGraph* renderGraph);
    void LinkDescriptorSet(RenderGraph* renderGraph);

public:
    VulkanDescriptorSet* GetDescriptorSet();

    void ApplyPipelineState(RHIPipelineState& state);

private:
    std::unordered_map<VkShaderStageFlagBits, VulkanShader*> _shaders;
    std::vector<TextureBinding> _textures;
    
    PassResource* _owner;

public:
    VkPipelineRasterizationStateCreateInfo rasterizeationState;
    VkPipelineDepthStencilStateCreateInfo depthStencilState;
    std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;
    VkPipelineColorBlendStateCreateInfo colorBlendState;
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

    VulkanDescriptorSetLayout* _descriptorSetLayout;
    VulkanDescriptorSet* _descriptorSet;
};

class Material : public RenderResource
{
public:
    Material(MaterialResource* material);
    ~Material();

    void SetPass(EPassType passType, Pass* pass);
    Pass* GetPass(EPassType passType);

private:
    std::unordered_map<EPassType, Pass*> _passMap;
    MaterialResource* _owner;
};

} // namespace ZE
