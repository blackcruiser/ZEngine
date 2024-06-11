#include <array>

#include "Material.h"
#include "Mesh.h"
#include "RenderSystem.h"
#include "Graphic/VulkanBuffer.h"
#include "Graphic/VulkanBufferManager.h"
#include "Graphic/VulkanDescriptorPool.h"
#include "Graphic/VulkanDescriptorSetLayout.h"
#include "Graphic/VulkanDescriptorSet.h"
#include "Graphic/VulkanImage.h"
#include "Graphic/VulkanImageView.h"
#include "Graphic/VulkanSampler.h"
#include "Graphic/VulkanPipelineLayout.h"
#include "Graphic/VulkanPipeline.h"
#include "Graphic/VulkanShader.h"
#include "Graphic/VulkanCommandBuffer.h"
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

Pass::Pass(TPtr<PassResource> passResource)
    : _owner(passResource), _descriptorSet(nullptr), _pipelineLayout(nullptr)
{
    for (const BlendState& blendState : passResource->GetBlendStates())
    {
        RHIBlendState outBlendState = ConvertBlendStateToVulkan(blendState);
        blendStates.push_back(outBlendState);
    }

    depthStencilState = ConvertDepthStencilStateToVulkan(passResource->GetDepthStencilState());
    rasterizationState.cullingType = ConvertCullingTypeToVulkanBit(passResource->GetCullingType());

    for (auto [shaderStage, shaderResource] : passResource->GetShaderMap())
    {
        RHIShaderState shaderState;

        shaderState.name = "main";
        shaderState.shaderModule = VK_NULL_HANDLE;
        shaderState.stage = ConvertShaderStageToVulkan(shaderStage);

        shaderStates.push_back(shaderState);
    }
}

Pass::~Pass()
{
}

void Pass::BuildRenderResource(TPtr<VulkanCommandBuffer> commandBuffer)
{
    CreateGraphicTextures(commandBuffer);
    CreateGraphicBuffers(commandBuffer);
    CreateGraphicShaders();

    CreateDescriptorSetLayout();
    CreateDescriptorSet();
    LinkDescriptorSet();

    CreatePipelineLayout();
}



TPtr<VulkanImageView> CreateGraphicImage(TPtr<VulkanDevice> device, TPtr<VulkanCommandBuffer> commandBuffer, TPtr<TextureResource> texture)
{
    assert(texture->IsLoaded());

    uint32_t imageSize = texture->GetWidth() * texture->GetHeight() * 4;
    VkExtent3D extent{texture->GetWidth(), texture->GetHeight(), 1};

    TPtr<VulkanBuffer> stagingBuffer = RenderSystem::Get().GetBufferManager()->AcquireStagingBuffer(imageSize);
    TPtr<VulkanImage> vulkanImage = std::make_shared<VulkanImage>(device, extent, VkFormat::VK_FORMAT_R8G8B8A8_SRGB);
    vulkanImage->TransferData(commandBuffer, stagingBuffer, texture->GetData(), imageSize);
    RenderSystem::Get().GetBufferManager()->ReleaseStagingBuffer(stagingBuffer, commandBuffer);

    TPtr<VulkanImageView> vulkanImageView = std::make_shared<VulkanImageView>(vulkanImage);

    return vulkanImageView;
}


void Pass::CreateGraphicTextures(TPtr<VulkanCommandBuffer> commandBuffer)
{
    assert(_owner.expired() == false);

    TPtr<PassResource> passResource = _owner.lock();
    TPtr<VulkanDevice> device = RenderSystem::Get().GetDevice();

    const std::unordered_map<EShaderStage, std::list<TextureBindingInfo>>& textureMap =
        passResource->GetTextureMap();

    for (auto& [stage, textureList] : textureMap)
    {
        if (textureList.empty() == true)
            continue;

        std::list<VulkanImageBindingInfo> vulkanBindingInfoList;
        for (const TextureBindingInfo& bindingInfo : textureList)
        {
            VulkanImageBindingInfo vulkanBindingInfo;

            vulkanBindingInfo.bindingPoint = bindingInfo.bindingPoint;
            vulkanBindingInfo.vulkanImageView = CreateGraphicImage(device, commandBuffer, bindingInfo.texture);
            vulkanBindingInfo.vulkanSampler = std::make_shared<VulkanSampler>(device);

            vulkanBindingInfoList.push_back(vulkanBindingInfo);
        }

        VkShaderStageFlagBits vulkanBit = ConvertShaderStageToVulkanBit(stage);
        _textures.insert(std::make_pair(vulkanBit, vulkanBindingInfoList));
    }
}

void Pass::CreateGraphicBuffers(TPtr<VulkanCommandBuffer> commandBuffer)
{
    _uniformBuffer = std::make_shared<VulkanBuffer>(commandBuffer->GetDevice(), sizeof(glm::mat4x4), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}

TPtr<VulkanShader> CreateGraphicShader(TPtr<VulkanDevice> device, VkShaderStageFlagBits shaderStage,
                                       TPtr<ShaderResource> shader)
{
    if (shader == nullptr)
        return nullptr;

    return std::make_shared<VulkanShader>(device, shader->GetByteCode());
}

void Pass::CreateGraphicShaders()
{
    assert(_owner.expired() == false);
    TPtr<PassResource> passResource = _owner.lock();

    TPtr<VulkanDevice> device = RenderSystem::Get().GetDevice();
    const TPtrUnorderedMap<EShaderStage, ShaderResource>& shaderMap = passResource->GetShaderMap();
    for (auto& [stage, shader] : shaderMap)
    {
        VkShaderStageFlagBits vulkanBit = ConvertShaderStageToVulkanBit(stage);
        TPtr<VulkanShader> vulkanShader = CreateGraphicShader(device, vulkanBit, shader);
        _shaders.insert(std::make_pair(vulkanBit, vulkanShader));

        for (RHIShaderState& shaderState : shaderStates)
        {
            if (shaderState.stage == vulkanBit)
            {
                shaderState.shaderModule = vulkanShader->GetRawShader();
            }
        }
    }
}

void Pass::CreateDescriptorSetLayout()
{
    TPtr<VulkanDevice> device = RenderSystem::Get().GetDevice();

    VkDescriptorSetLayoutBinding matrixDescriptorSetlayoutBinding{};
    matrixDescriptorSetlayoutBinding.binding = 0;
    matrixDescriptorSetlayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    matrixDescriptorSetlayoutBinding.descriptorCount = 1;
    matrixDescriptorSetlayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutBinding samplerDescriptorSetlayoutBinding{};
    samplerDescriptorSetlayoutBinding.binding = 1;
    samplerDescriptorSetlayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerDescriptorSetlayoutBinding.descriptorCount = 1;
    samplerDescriptorSetlayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::vector<VkDescriptorSetLayoutBinding> localDescriptorSetLayoutBindings = {
        matrixDescriptorSetlayoutBinding, samplerDescriptorSetlayoutBinding};

    _descriptorSetLayout = std::make_shared<VulkanDescriptorSetLayout>(device, localDescriptorSetLayoutBindings);
}

void Pass::CreateDescriptorSet()
{
    TPtr<VulkanDescriptorPool> descriptorPool = RenderSystem::Get().GetDescriptorPool();

    _descriptorSet = std::make_shared<VulkanDescriptorSet>(descriptorPool, _descriptorSetLayout);
}

void Pass::LinkDescriptorSet()
{
    {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = _uniformBuffer->GetRawBuffer();
        bufferInfo.offset = 0;
        bufferInfo.range = _uniformBuffer->GetSize();

        _descriptorSet->Update(0, 0, bufferInfo);
    }

    {
        if (_textures.find(VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT) == _textures.end())
            return;

        std::list<VulkanImageBindingInfo>& textureBindingInfo = _textures.at(VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT);

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = textureBindingInfo.begin()->vulkanImageView->GetRawImageView();
        imageInfo.sampler = textureBindingInfo.begin()->vulkanSampler->GetRawSampler();

        _descriptorSet->Update(1, 0, imageInfo);
    }
}

void Pass::CreatePipelineLayout()
{
    TPtr<VulkanDevice> device = RenderSystem::Get().GetDevice();

    TPtrArr<VulkanDescriptorSetLayout> descriptorSetLayoutArr{_descriptorSetLayout};
    _pipelineLayout = std::make_shared<VulkanPipelineLayout>(device, descriptorSetLayoutArr);
}

TPtr<VulkanDescriptorSet> Pass::GetDescriptorSet()
{
    return _descriptorSet;
}

TPtr<VulkanPipelineLayout> Pass::GetPipelineLayout()
{
    return _pipelineLayout;
}

void Pass::ApplyPipelineState(RHIPipelineState& state)
{
    VkPipelineDepthStencilStateCreateInfo& depthStencil = state.depthStencilState;
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = depthStencilState.depthTestEnable;
    depthStencil.depthWriteEnable = depthStencilState.depthWriteEnable;
    depthStencil.depthCompareOp = depthStencilState.depthCompareOp;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;

    std::vector<VkPipelineShaderStageCreateInfo>& shaderStages = state.shaderStages;
    for (const RHIShaderState& shaderState : shaderStates)
    {
        VkPipelineShaderStageCreateInfo vkFragmentShaderStageCreateInfo{};
        vkFragmentShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vkFragmentShaderStageCreateInfo.stage = shaderState.stage;
        vkFragmentShaderStageCreateInfo.pName = shaderState.name.c_str();
        vkFragmentShaderStageCreateInfo.module = shaderState.shaderModule;

        shaderStages.push_back(vkFragmentShaderStageCreateInfo);
    }

    VkPipelineRasterizationStateCreateInfo& rasterizer = state.rasterizeationState;
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode =  rasterizationState.cullingType;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp = 0.0f;          // Optional
    rasterizer.depthBiasSlopeFactor = 0.0f;    // Optional

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;             // Optional
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;             // Optional
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
    state.colorBlendAttachments.push_back(colorBlendAttachment);

    VkPipelineColorBlendStateCreateInfo& colorBlending = state.colorBlendState;
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlending.attachmentCount = static_cast<uint32_t>(state.colorBlendAttachments.size());
    colorBlending.pAttachments = state.colorBlendAttachments.data();
    colorBlending.blendConstants[0] = 0.0f; // Optional
    colorBlending.blendConstants[1] = 0.0f; // Optional
    colorBlending.blendConstants[2] = 0.0f; // Optional
    colorBlending.blendConstants[3] = 0.0f; // Optional

    state.layout = _pipelineLayout->GetRawPipelineLayout();
}

void Pass::UpdateUniformBuffer(TPtr<VulkanCommandBuffer> commandBuffer, const glm::mat4x4& mvp)
{
    TPtr<VulkanBuffer> stagingBuffer = RenderSystem::Get().GetBufferManager()->AcquireStagingBuffer(sizeof(mvp));
    _uniformBuffer->TransferData(commandBuffer, stagingBuffer, &mvp, sizeof(mvp));
    RenderSystem::Get().GetBufferManager()->ReleaseStagingBuffer(stagingBuffer, commandBuffer);
}

Material::Material(TPtr<MaterialResource> materialResource)
    : _owner(materialResource)
{
}

Material::~Material()
{
}

void Material::SetPass(EPassType passType, TPtr<Pass> pass)
{
    _passMap.insert(std::make_pair(passType, pass));
}

TPtr<Pass> Material::GetPass(EPassType passType)
{
    if (_passMap.find(passType) == _passMap.end())
        return nullptr;
    else
        return _passMap[passType];
}

} // namespace ZE
