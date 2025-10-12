#include <array>

#include "Material.h"
#include "Mesh.h"
#include "RenderSystem.h"
#include "Render/RenderGraph.h"
#include "Render/Texture.h"
#include "Graphic/VulkanBuffer.h"
#include "Graphic/VulkanStagingBufferManager.h"
#include "Graphic/VulkanDescriptorPool.h"
#include "Graphic/VulkanDescriptorSetLayout.h"
#include "Graphic/VulkanDescriptorSet.h"
#include "Graphic/VulkanImage.h"
#include "Graphic/VulkanSampler.h"
#include "Graphic/VulkanPipelineLayout.h"
#include "Graphic/VulkanPipeline.h"
#include "Graphic/VulkanShader.h"
#include "Graphic/VulkanCommandBuffer.h"
#include "Graphic/GraphicResource.h"
#include "Resource/ShaderResource.h"
#include "Resource/TextureResource.h"


namespace ZE {

VkShaderStageFlagBits ConvertShaderStageToVulkanBit(EShaderStage stage)
{
    switch (stage)
    {
    case EShaderStage::Vertex:
        return VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
    case EShaderStage::Fragment:
        return VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;

    default:
        return VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
    }
}

VkCullModeFlagBits ConvertCullingTypeToVulkanBit(ECullingType cullingType)
{
    switch ( cullingType)
    {
    case ECullingType::None:
        return VkCullModeFlagBits::VK_CULL_MODE_NONE;
    case ECullingType::Font:
        return VkCullModeFlagBits::VK_CULL_MODE_FRONT_BIT;
    case ECullingType::Back:
        return VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT;
    default:
        return VkCullModeFlagBits::VK_CULL_MODE_NONE;
    }
}

VkBlendFactor ConvertBlendFactorToVulkan(EBlendFactor blendFactor)
{
    switch (blendFactor)
    {
    case EBlendFactor::Zero:
        return VkBlendFactor::VK_BLEND_FACTOR_ZERO;
    case EBlendFactor::One:
        return VkBlendFactor::VK_BLEND_FACTOR_ONE;
    case EBlendFactor::SrcColor:
        return VkBlendFactor::VK_BLEND_FACTOR_SRC_COLOR;
    case EBlendFactor::SrcAlpha:
        return VkBlendFactor::VK_BLEND_FACTOR_SRC_ALPHA;
    case EBlendFactor::DstColor:
        return VkBlendFactor::VK_BLEND_FACTOR_DST_COLOR;
    case EBlendFactor::DstAlpha:
        return VkBlendFactor::VK_BLEND_FACTOR_DST_ALPHA;
    case EBlendFactor::OneMinusSrcColor:
        return VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
    case EBlendFactor::OneMinusSrcAlpha:
        return VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    case EBlendFactor::OneMinusDstColor:
        return VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
    case EBlendFactor::OneMinusDstAlpha:
        return VkBlendFactor::VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
    }

    return VkBlendFactor::VK_BLEND_FACTOR_ZERO;
}

VkBlendOp ConvertBlendOperationToVulkan(EBlendOperation blendOperation)
{
    switch (blendOperation)
    {
    case EBlendOperation::Add:
        return VkBlendOp::VK_BLEND_OP_ADD;
    case EBlendOperation::Sub:
        return VkBlendOp::VK_BLEND_OP_SUBTRACT;
    }

    return VkBlendOp::VK_BLEND_OP_ADD;
}

VkCompareOp ConvertCompareOperationToVulkan(ECompareOperation compareOperation)
{
    switch (compareOperation)
    {
    case ECompareOperation::Never:
        return VkCompareOp::VK_COMPARE_OP_NEVER;
    case ECompareOperation::Less:
        return VkCompareOp::VK_COMPARE_OP_LESS;
    case ECompareOperation::Equal:
        return VkCompareOp::VK_COMPARE_OP_EQUAL;
    case ECompareOperation::LEqual:
        return VkCompareOp::VK_COMPARE_OP_LESS_OR_EQUAL;
    case ECompareOperation::Greater:
        return VkCompareOp::VK_COMPARE_OP_GREATER;
    case ECompareOperation::NotEqual:
        return VkCompareOp::VK_COMPARE_OP_NOT_EQUAL;
    case ECompareOperation::GEqual:
        return VkCompareOp::VK_COMPARE_OP_GREATER_OR_EQUAL;
    case ECompareOperation::Always:
        return VkCompareOp::VK_COMPARE_OP_ALWAYS;
    }

    return VkCompareOp::VK_COMPARE_OP_NEVER;
}

RHIBlendState ConvertBlendStateToVulkan(const BlendState& blendState)
{
    RHIBlendState outBlendState;

    outBlendState.srcFactor = ConvertBlendFactorToVulkan(blendState.srcFactor);
    outBlendState.dstFactor = ConvertBlendFactorToVulkan(blendState.dstFactor);
    outBlendState.srcAlphaFactor = ConvertBlendFactorToVulkan(blendState.srcAlphaFactor);
    outBlendState.dstAlphaFactor = ConvertBlendFactorToVulkan(blendState.dstAlphaFactor);

    outBlendState.operation = ConvertBlendOperationToVulkan(blendState.operation);

    return outBlendState;
}

VkStencilOp ConvertStencilOperationToVulkan(EStencilOperation stencilOperation)
{
    switch (stencilOperation)
    {
    case EStencilOperation::Keep:
        return VkStencilOp::VK_STENCIL_OP_KEEP;
    case EStencilOperation::Zero:
        return VkStencilOp::VK_STENCIL_OP_ZERO;
    case EStencilOperation::Replace:
        return VkStencilOp::VK_STENCIL_OP_REPLACE;
    case EStencilOperation::IncreaseSaturate:
        return VkStencilOp::VK_STENCIL_OP_INCREMENT_AND_CLAMP;
    case EStencilOperation::DecreaseSaturate:
        return VkStencilOp::VK_STENCIL_OP_DECREMENT_AND_CLAMP;
    case EStencilOperation::Invert:
        return VkStencilOp::VK_STENCIL_OP_INVERT;
    case EStencilOperation::IncreaseWrap:
        return VkStencilOp::VK_STENCIL_OP_INCREMENT_AND_WRAP;
    case EStencilOperation::DecreaseWrap:
        return VkStencilOp::VK_STENCIL_OP_DECREMENT_AND_WRAP;
    }

    return VkStencilOp::VK_STENCIL_OP_KEEP;
}

VkStencilOpState ConvertStencilOperationStateToVulkan(const StencilOperationState& stencilOperationState)
{
    VkStencilOpState outStencilOpState;

    outStencilOpState.failOp = ConvertStencilOperationToVulkan(stencilOperationState.failOperation);
    outStencilOpState.passOp = ConvertStencilOperationToVulkan(stencilOperationState.passOperation);
    outStencilOpState.depthFailOp = ConvertStencilOperationToVulkan(stencilOperationState.depthFailOperation);

    outStencilOpState.compareOp = ConvertCompareOperationToVulkan(stencilOperationState.compareFunction);

    outStencilOpState.compareMask = stencilOperationState.readMask;
    outStencilOpState.writeMask = stencilOperationState.writeMask;
    outStencilOpState.reference = stencilOperationState.ref;

    return outStencilOpState;
}

RHIDepthStencilState ConvertDepthStencilStateToVulkan(const DepthStencilState& depthStencilState)
{
    RHIDepthStencilState outDepthStencilState;

    outDepthStencilState.front = ConvertStencilOperationStateToVulkan(depthStencilState.front);
    outDepthStencilState.back = ConvertStencilOperationStateToVulkan(depthStencilState.back);
    outDepthStencilState.stencilTestEnable = depthStencilState.front.compareFunction != ECompareOperation::Never && depthStencilState.back.compareFunction != ECompareOperation::Never;

    outDepthStencilState.depthTestEnable = depthStencilState.zTestType == ECompareOperation::Never ? 0 : 1;
    outDepthStencilState.depthWriteEnable = depthStencilState.zWriteType == EZWriteType::Enable ? 1 : 0;
    outDepthStencilState.depthCompareOp = ConvertCompareOperationToVulkan(depthStencilState.zTestType);

    return outDepthStencilState;
}

VkShaderStageFlagBits ConvertShaderStageToVulkan(EShaderStage shaderStage)
{
    switch (shaderStage)
    {
    case EShaderStage::Vertex:
        return VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
    case EShaderStage::Fragment:
        return VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
    }

    return VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
}

Pass::Pass(PassResource* passResource)
    : _owner(passResource), _descriptorSet(nullptr)
{
    // for (const BlendState& blendState : passResource->GetBlendStates())
    // {
    //     RHIBlendState outBlendState = ConvertBlendStateToVulkan(blendState);
    //     blendStates.push_back(outBlendState);
    // }

    // depthStencilState = ConvertDepthStencilStateToVulkan(passResource->GetDepthStencilState());
    // rasterizationState.cullingType = ConvertCullingTypeToVulkanBit(passResource->GetCullingType());

    // for (auto [shaderStage, shaderResource] : passResource->GetShaderMap())
    // {
    //     RHIShaderState shaderState;

    //     shaderState.name = "main";
    //     shaderState.shaderModule = VK_NULL_HANDLE;
    //     shaderState.stage = ConvertShaderStageToVulkan(shaderStage);

    //     shaderStates.push_back(shaderState);
    // }
}

Pass::~Pass()
{
}

void Pass::InitGraphic(RenderGraph* renderGraph)
{
    RenderResource::InitGraphic(renderGraph);

    CreateGraphicShaders(renderGraph);

    CreateDescriptorSetLayout(renderGraph);
    CreateDescriptorSet(renderGraph);
    LinkDescriptorSet(renderGraph);

    renderGraph->Execute();
}

void Pass::CleanupGraphic()
{
    delete _descriptorSet;
    delete _descriptorSetLayout;

    for (auto iter = _shaders.begin(); iter != _shaders.end(); iter++)
    {
        delete iter->second;
    }

    RenderResource::CleanupGraphic();
}

VulkanShader* CreateGraphicShader(VulkanDevice* device, VkShaderStageFlagBits shaderStage,
                                       TPtr<ShaderResource> shader)
{
    if (shader == nullptr)
        return nullptr;

    return new VulkanShader(device, shader->GetByteCode());
}

void Pass::CreateGraphicShaders(RenderGraph* renderGraph)
{
    assert(_owner != nullptr);

    VulkanDevice* device = renderGraph->GetDevice();
    const TPtrUnorderedMap<EShaderStage, ShaderResource>& shaderMap = _owner->GetShaderMap();
    for (auto& [stage, shader] : shaderMap)
    {
        VkShaderStageFlagBits vulkanBit = ConvertShaderStageToVulkanBit(stage);
        VulkanShader* vulkanShader = CreateGraphicShader(device, vulkanBit, shader);
        _shaders.insert(std::make_pair(vulkanBit, vulkanShader));

        VkPipelineShaderStageCreateInfo createInfo{};
        createInfo.stage = ConvertShaderStageToVulkanBit(stage);
        createInfo.module = vulkanShader->GetRawShader();

        shaderStages.push_back(createInfo);
    }
}

void Pass::CreateDescriptorSetLayout(RenderGraph* renderGraph)
{
    std::vector<VkDescriptorSetLayoutBinding> localDescriptorSetLayoutBindings;
    for (TextureBinding& bindingInfo : _textures)
    {
        VkDescriptorSetLayoutBinding samplerDescriptorSetlayoutBinding{};
        samplerDescriptorSetlayoutBinding.binding = 1;
        samplerDescriptorSetlayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerDescriptorSetlayoutBinding.descriptorCount = 1;
        samplerDescriptorSetlayoutBinding.stageFlags = bindingInfo.flagBits;

        localDescriptorSetLayoutBindings.push_back(samplerDescriptorSetlayoutBinding);
    }

    VulkanDevice* device = renderGraph->GetDevice();
    _descriptorSetLayout = new VulkanDescriptorSetLayout(device, localDescriptorSetLayoutBindings);
}

void Pass::CreateDescriptorSet(RenderGraph* renderGraph)
{
    VulkanDescriptorPool* descriptorPool = RenderSystem::Get().GetDescriptorPool();
    _descriptorSet = new VulkanDescriptorSet(renderGraph->GetDevice(), descriptorPool, _descriptorSetLayout);

    for (TextureBinding& bindingInfo : _textures)
    {
        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = bindingInfo.texture->GetImage()->GetRawImageView();
        imageInfo.sampler = bindingInfo.texture->GetSampler()->GetRawSampler();

        _descriptorSet->Update(1, 0, imageInfo);
    }
}

VulkanDescriptorSet* Pass::GetDescriptorSet()
{
    return _descriptorSet;
}

Material::Material(MaterialResource* materialResource)
    : _owner(materialResource)
{
}

Material::~Material()
{
}

void Material::SetPass(EPassType passType, Pass* pass)
{
    _passMap .insert(std::make_pair(passType, pass));
}

Pass* Material::GetPass(EPassType passType)
{
    if (_passMap.find(passType) == _passMap.end())
        return nullptr;
    else
        return _passMap[passType];
}

} // namespace ZE
