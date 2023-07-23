#pragma once

#include "Renderer.h"
#include "Graphic/Window.h"

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>


namespace ZE {

class SceneObject;
class DirectionalLightPass;
class DepthPass;
class VulkanCommandBuffer;
class VulkanDevice;
class Surface;


class ForwardRenderer : public RendererInterface
{
public:
    ForwardRenderer();
    virtual ~ForwardRenderer();

    virtual void Init(TPtr<Scene> scene) override;
    TPtrArr<SceneObject> Prepare(TPtr<VulkanCommandBuffer> commandBuffer, TPtr<Scene> scene);
    void Draw(TPtr<VulkanCommandBuffer> commandBuffer, TPtr<Scene> scene);
    virtual void RenderFrame(TPtr<Scene> scene, TPtr<Window> window) override;

private:
    VkSemaphore _imageAvailableSemaphore, _renderFinishedSemaphore;
    VkFence _inFlightFence;

    TPtr<DepthPass> _depthPass;
    TPtr<DirectionalLightPass> _directionalLightPass;
};

}