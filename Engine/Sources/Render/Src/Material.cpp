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
#include "Resource/MaterialResource.h"
#include "Resource/ShaderResource.h"
#include "Resource/TextureResource.h"


namespace ZE {

Material::Material(TPtr<MaterialResource> materialResource)
    : _owner(materialResource), _descriptorSet(nullptr), _pipelineLayout(nullptr)
{
}

Material::~Material()
{
}

void Material::BuildRenderResource(TPtr<VulkanCommandBuffer> commandBuffer)
{
    CreateGraphicTextures(commandBuffer);
    CreateGraphicBuffers(commandBuffer);
    CreateGraphicShaders();

    CreateDescriptorSetLayout();
    CreateDescriptorSet();
    LinkDescriptorSet();

    CreatePipelineLayout();
}


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


void Material::CreateGraphicTextures(TPtr<VulkanCommandBuffer> commandBuffer)
{
    assert(_owner.expired() == false);

    TPtr<MaterialResource> materialResource = _owner.lock();
    TPtr<VulkanDevice> device = RenderSystem::Get().GetDevice();

    const std::unordered_map<EShaderStage, std::list<TextureBindingInfo>>& textureMap =
        materialResource->GetTextureMap();

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

void Material::CreateGraphicBuffers(TPtr<VulkanCommandBuffer> commandBuffer)
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

void Material::CreateGraphicShaders()
{
    assert(_owner.expired() == false);
    TPtr<MaterialResource> materialResource = _owner.lock();

    TPtr<VulkanDevice> device = RenderSystem::Get().GetDevice();
    const TPtrUnorderedMap<EShaderStage, ShaderResource>& shaderMap = materialResource->GetShaderMap();
    for (auto& [stage, shader] : shaderMap)
    {
        VkShaderStageFlagBits vulkanBit = ConvertShaderStageToVulkanBit(stage);
        TPtr<VulkanShader> vulkanShader = CreateGraphicShader(device, vulkanBit, shader);
        _shaders.insert(std::make_pair(vulkanBit, vulkanShader));
    }
}

void Material::CreateDescriptorSetLayout()
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

void Material::CreateDescriptorSet()
{
    TPtr<VulkanDescriptorPool> descriptorPool = RenderSystem::Get().GetDescriptorPool();

    _descriptorSet = std::make_shared<VulkanDescriptorSet>(descriptorPool, _descriptorSetLayout);
}

void Material::LinkDescriptorSet()
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

void Material::CreatePipelineLayout()
{
    TPtr<VulkanDevice> device = RenderSystem::Get().GetDevice();

    TPtrArr<VulkanDescriptorSetLayout> descriptorSetLayoutArr{_descriptorSetLayout};
    _pipelineLayout = std::make_shared<VulkanPipelineLayout>(device, descriptorSetLayoutArr);
}

TPtr<VulkanDescriptorSet> Material::GetDescriptorSet()
{
    return _descriptorSet;
}

TPtr<VulkanPipelineLayout> Material::GetPipelineLayout()
{
    return _pipelineLayout;
}

void Material::BuildPipelineDesc(VulkanGraphicPipelineDesc& desc)
{
    if (_shaders.find(VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT) != _shaders.end())
        desc.vertexShader = _shaders[VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT];
    else
        desc.vertexShader = nullptr;

    if (_shaders.find(VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT) != _shaders.end())
        desc.fragmentShader = _shaders[VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT];
    else
        desc.fragmentShader = nullptr;

    desc.pipelineLayout = _pipelineLayout;
}

void Material::UpdateUniformBuffer(TPtr<VulkanCommandBuffer> commandBuffer, const glm::mat4x4& mvp)
{
    TPtr<VulkanBuffer> stagingBuffer = RenderSystem::Get().GetBufferManager()->AcquireStagingBuffer(sizeof(mvp));
    _uniformBuffer->TransferData(commandBuffer, stagingBuffer, &mvp, sizeof(mvp));
    RenderSystem::Get().GetBufferManager()->ReleaseStagingBuffer(stagingBuffer, commandBuffer);
}

} // namespace ZE
