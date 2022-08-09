#pragma once

#include "Renderer.h"
#include "Graphic/Device.h"
#include "Graphic/Window.h"

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>


namespace TE {

class SceneObject;
class MaterialComponent;
class CommandPool;
class CommandBuffer;


class ForwardRenderer : public RendererInterface
{
public:
    ForwardRenderer(TPtr<Device> device, TPtr<Surface> surface);
    virtual ~ForwardRenderer();

    VkPipeline CreatePipeline(TPtr<MaterialComponent> material);
    void CreateSwapchain(VkRenderPass renderPass);

    void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    void GatherObjects(TPtr<Scene> scene);
    virtual void RenderFrame(TPtr<Scene> scene) override;

private:
    std::map<std::uintptr_t, TPtrArr<SceneObject>> _objectsToRender;

    TPtr<Device> _device;
    TPtr<Surface> _surface;

    VkSwapchainKHR _vkSwapchain;
    std::vector<VkFramebuffer> _vkFramebuffers;
    std::vector<VkImage> _vkImages;
    std::vector<VkImageView> _vkImageViews;
    std::vector<VkShaderModule> _vkShaderModules;

    VkRenderPass _vkRenderPass;
    VkPipelineLayout _vkPipelineLayout;
    std::map<size_t, VkPipeline> _pipelines;

    VkBuffer _stagingBuffer, _vertexBuffer, _indexBuffer, _uniformBuffer;
    VkDeviceMemory _stagingBufferMemory, _vertexBufferMemory, _indexBufferMemory, _uniformBufferMemory;
    size_t _stagingBufferSize, _vertexBufferSize, _indexesBufferSize;

    VkSemaphore _imageAvailableSemaphore, _renderFinishedSemaphore;
    VkFence _inFlightFence;

    VkDescriptorPool _descriptorPool;
    VkDescriptorSetLayout _descriptorLayout;
    VkDescriptorSet _descriptorSet;

    TPtr<CommandPool> _commandPool;
    CommandBuffer* _commandBuffer;
};

}