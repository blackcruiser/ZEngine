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
    ForwardRenderer();
    virtual ~ForwardRenderer();

    virtual void Init(TPtr<Scene> scene) override;
    virtual void RenderFrame(TPtr<Scene> scene, TPtr<Window> window) override;

private:
    VkSemaphore _imageAvailableSemaphore, _renderFinishedSemaphore;
    VkFence _inFlightFence;
};

}