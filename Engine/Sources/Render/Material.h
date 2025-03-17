#include "CoreDefines.h"
#include "CoreTypes.h"
#include "Graphic/VulkanDevice.h"

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
class VulkanPipelineLayout;
class VulkanGraphicPipeline;
class VulkanRenderPass;
class VulkanCommandPool;
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
    Material(TPtr<VulkanDevice> device, TPtr<MaterialResource> material);
    ~Material();

    void CreateGraphicTextures(TPtr<VulkanCommandPool> commandPool);
    std::optional<std::reference_wrapper<std::list<VulkanImageBindingInfo>>> GetGraphicTexture(
        VkShaderStageFlagBits stage);

    void CreateGraphicShaders();
    TPtr<VulkanShader> GetGraphicShader(VkShaderStageFlagBits stage);

    void UpdateUniformBuffer(TPtr<VulkanCommandPool> commandPool, const glm::mat4x4& transform);
    TPtr<VulkanBuffer> GetUniformBuffer();

    void CreatePipeline(TPtr<Mesh> mesh, TPtr<VulkanRenderPass> renderPass,  const VkExtent2D& extent);
    TPtr<VulkanPipelineLayout> GetPipelineLayout();
    TPtr<VulkanGraphicPipeline> GetPipeline();

    void CreateDescriptorSet(TPtr<VulkanDescriptorPool> descriptorPool);
    TPtr<VulkanDescriptorSet> GetDescriptorSet();
    void UpdateDescriptorSet();

private:
    TPtrUnorderedMap<VkShaderStageFlagBits, VulkanShader> _shaders;
    std::unordered_map<VkShaderStageFlagBits, std::list<VulkanImageBindingInfo>> _textures;
    TPtr<VulkanBuffer> _uniformBuffer;
    TPtr<VulkanDescriptorSet> _descriptorSet;
    TPtr<VulkanPipelineLayout> _pipelineLayout;
    TPtr<VulkanGraphicPipeline> _pipeline;

    TPtr<VulkanDevice> _device;
    TWeakPtr<MaterialResource> _owner;
};

} // namespace ZE
