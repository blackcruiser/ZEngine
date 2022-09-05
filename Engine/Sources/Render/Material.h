#include "CoreDefines.h"
#include "Graphic/VulkanDevice.h"

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

#include <optional>

namespace TE {

class VulkanImage;
class VulkanShader;
class VulkanBuffer;
class VulkanDescriptorPool;
class VulkanDescriptorSet;
class VulkanPipelineLayout;
class VulkanGraphicPipeline;
class VulkanCommandPool;
class MaterialResource;
class Mesh;

struct VulkanImageBindingInfo
{
    uint32_t bindingPoint;
    TPtr<VulkanImage> vulkanImage;
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

    void CreatePipeline(TPtr<Mesh> mesh, const VkExtent2D& extent, VkRenderPass renderPass);
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

} // namespace TE