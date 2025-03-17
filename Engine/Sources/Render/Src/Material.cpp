#include "Material.h"
#include "Graphic/VulkanBuffer.h"
#include "Graphic/VulkanDescriptorPool.h"
#include "Graphic/VulkanDescriptorSet.h"
#include "Graphic/VulkanImage.h"
#include "Graphic/VulkanPipelineLayout.h"
#include "Graphic/VulkanPipeline.h"
#include "Graphic/VulkanShader.h"
#include "Mesh.h"
#include "Resource/MaterialResource.h"
#include "Resource/ShaderResource.h"
#include "Resource/TextureResource.h"


namespace TE {

Material::Material(TPtr<VulkanDevice> device, TPtr<MaterialResource> materialResource)
    : _device(device), _owner(materialResource), _descriptorSet(nullptr), _pipelineLayout(nullptr), _pipeline(nullptr)
{
    _uniformBuffer =
        std::make_shared<VulkanBuffer>(device, sizeof(glm::mat4x4), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

Material::~Material()
{
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

TPtr<VulkanImage> CreateGraphicImage(TPtr<VulkanDevice> device, TPtr<VulkanCommandPool> commandPool, TPtr<TextureResource> texture)
{
    assert(texture->IsLoaded());

    TPtr<VulkanImage> vulkanImage =
        std::make_shared<VulkanImage>(device, texture->GetWidth(), texture->GetHeight(), VK_FORMAT_R8G8B8A8_SRGB);

    uint32_t imageSize = texture->GetWidth() * texture->GetHeight() * 4;
    vulkanImage->UploadData(commandPool, texture->GetData(), imageSize);

    return vulkanImage;
}


void Material::CreateGraphicTextures(TPtr<VulkanCommandPool> commandPool)
{
    assert(_owner.expired() == false);

    TPtr<MaterialResource> materialResource = _owner.lock();

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
            vulkanBindingInfo.vulkanImage = CreateGraphicImage(_device, commandPool, bindingInfo.texture);

            vulkanBindingInfoList.push_back(vulkanBindingInfo);
        }

        VkShaderStageFlagBits vulkanBit = ConvertShaderStageToVulkanBit(stage);
        _textures.insert(std::make_pair(vulkanBit, vulkanBindingInfoList));
    }
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

    const TPtrUnorderedMap<EShaderStage, ShaderResource>& shaderMap = materialResource->GetShaderMap();
    for (auto& [stage, shader] : shaderMap)
    {
        VkShaderStageFlagBits vulkanBit = ConvertShaderStageToVulkanBit(stage);
        TPtr<VulkanShader> vulkanShader = CreateGraphicShader(_device, vulkanBit, shader);
        _shaders.insert(std::make_pair(vulkanBit, vulkanShader));
    }
}

TPtr<VulkanShader> Material::GetGraphicShader(VkShaderStageFlagBits stage)
{
    if (_shaders.find(stage) == _shaders.end())
        return nullptr;
    else
        return _shaders[stage];
}

std::optional<std::reference_wrapper<std::list<VulkanImageBindingInfo>>> Material::GetGraphicTexture(
    VkShaderStageFlagBits stage)
{
    if (_textures.find(stage) == _textures.end())
        return std::optional<std::reference_wrapper<std::list<VulkanImageBindingInfo>>>();
    else
        return _textures[stage];
}

void Material::UpdateUniformBuffer(TPtr<VulkanCommandPool> commandPool, const glm::mat4x4& transform)
{

    _uniformBuffer->UploadData(commandPool, &transform, sizeof(transform));
}

TPtr<VulkanBuffer> Material::GetUniformBuffer()
{
    return _uniformBuffer;
}

void Material::CreatePipeline(TPtr<Mesh> mesh, const VkExtent2D& extent, VkRenderPass renderPass)
{
    TPtr<VulkanShader> vertexShader = GetGraphicShader(VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT);
    TPtr<VulkanShader> fragmentShader = GetGraphicShader(VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT);

    VkVertexInputBindingDescription bindingDescription = mesh->GetVertexInputBindingDescription();
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions = mesh->GetVertexInputAttributeDescriptions();

    _pipelineLayout = std::make_shared<VulkanPipelineLayout>(_device, _descriptorSet);

    _pipeline = std::make_shared<VulkanGraphicPipeline>(_device, vertexShader, fragmentShader, extent, bindingDescription, attributeDescriptions, _pipelineLayout, renderPass);
}

TPtr<VulkanPipelineLayout> Material::GetPipelineLayout()
{
    return _pipelineLayout;
}

TPtr<VulkanGraphicPipeline> Material::GetPipeline()
{
    return _pipeline;
}

void Material::CreateDescriptorSet(TPtr<VulkanDescriptorPool> descriptorPool)
{
    VkDescriptorSetLayoutBinding uniformDescriptorSetLayoutBinding{};
    uniformDescriptorSetLayoutBinding.binding = 0;
    uniformDescriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uniformDescriptorSetLayoutBinding.descriptorCount = 1;
    uniformDescriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutBinding samplerDescriptorSetlayoutBinding{};
    samplerDescriptorSetlayoutBinding.binding = 1;
    samplerDescriptorSetlayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerDescriptorSetlayoutBinding.descriptorCount = 1;
    samplerDescriptorSetlayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings = {uniformDescriptorSetLayoutBinding,
                                                                             samplerDescriptorSetlayoutBinding};

    _descriptorSet = std::make_shared<VulkanDescriptorSet>(descriptorPool, descriptorSetLayoutBindings);
}

TPtr<VulkanDescriptorSet> Material::GetDescriptorSet()
{
    return _descriptorSet;
}

void Material::UpdateDescriptorSet()
{
    std::optional<std::reference_wrapper<std::list<VulkanImageBindingInfo>>> textureBindingInfoWrap =
        GetGraphicTexture(VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT);
    std::list<VulkanImageBindingInfo> textureBindingInfo = textureBindingInfoWrap.value();

    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = GetUniformBuffer()->GetRawBuffer();
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(glm::mat4x4);

    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = textureBindingInfo.begin()->vulkanImage->GetRawImageView();
    imageInfo.sampler = textureBindingInfo.begin()->vulkanImage->GetRawSampler();

    std::vector<VkDescriptorBufferInfo> bufferInfoArr{bufferInfo};
    std::vector<VkDescriptorImageInfo> imageInfoArr{imageInfo};

    _descriptorSet->Update(bufferInfoArr, imageInfoArr);
}

} // namespace TE
