#pragma once

#include "Renderer.h"
#include "Graphic/Window.h"

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>


namespace ZE {

class SceneObject;
class Mesh;
class Material;
class VulkanCommandPool;
class VulkanCommandBuffer;
class VulkanDescriptorPool;
class VulkanRenderPass;
class VulkanSwapchain;
class VulkanFramebuffer;
class VulkanDevice;
class Surface;


class ForwardRenderer : public RendererInterface
{
public:
    ForwardRenderer(TPtr<VulkanDevice> device, TPtr<Surface> surface);
    virtual ~ForwardRenderer();

    virtual void Init(TPtr<Scene> scene) override;
    virtual void RenderFrame(TPtr<Scene> scene) override;

private:
    TPtr<VulkanDevice> _device;
    TPtr<Surface> _surface;
    TPtr<VulkanRenderPass> _renderPass;
    TPtrArr<VulkanFramebuffer> _framebuffers;
    TPtr<VulkanSwapchain> _swapchain;
    TPtr<VulkanDescriptorPool> _descriptorPool;
    TPtr<VulkanCommandPool> _commandPool;
    TPtr<VulkanCommandBuffer> _commandBuffer;

    VkSemaphore _imageAvailableSemaphore, _renderFinishedSemaphore;
    VkFence _inFlightFence;
};

}