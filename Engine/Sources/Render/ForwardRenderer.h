#pragma once

#include "Renderer.h"
#include "Graphic/Device.h"
#include "Graphic/Window.h"

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>


namespace TE {

class SceneObject;
class MeshComponent;
class RSMaterial;
class CommandPool;
class CommandBuffer;


class ForwardRenderer : public RendererInterface
{
public:
    ForwardRenderer(TPtr<Device> device, TPtr<Surface> surface);
    virtual ~ForwardRenderer();

    void CreateDescriptorSet(TPtr<MeshComponent> meshComponent, TPtr<RSMaterial> material);
    void UpdateDescriptorSet(VkDescriptorSet descriptorSet, TPtr<MeshComponent> meshComponent,
                             TPtr<RSMaterial> material);

    VkPipeline CreatePipeline(TPtr<MeshComponent> meshComponent, TPtr<RSMaterial> material);
    void CreateSwapchain(VkRenderPass renderPass);

    void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    void TransitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);

    virtual void RenderFrame(TPtr<Scene> scene) override;

private:
    std::map<std::uintptr_t, TPtrArr<SceneObject>> _objectsToRender;
 
    TPtr<Device> _device;
    TPtr<Surface> _surface;

    VkSwapchainKHR _vkSwapchain;
    std::vector<VkFramebuffer> _vkFramebuffers;
    std::vector<VkImage> _vkImages;
    std::vector<VkImageView> _vkImageViews;

    VkRenderPass _vkRenderPass;

    VkBuffer _stagingBuffer, _vertexBuffer, _indexBuffer;
    VkDeviceMemory _stagingBufferMemory, _vertexBufferMemory, _indexBufferMemory;
    size_t _stagingBufferSize, _vertexBufferSize, _indexesBufferSize;

    VkSemaphore _imageAvailableSemaphore, _renderFinishedSemaphore;
    VkFence _inFlightFence;

    VkDescriptorPool _descriptorPool;

    TPtr<CommandPool> _commandPool;
    CommandBuffer* _commandBuffer;
};

}