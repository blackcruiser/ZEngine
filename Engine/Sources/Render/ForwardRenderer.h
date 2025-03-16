#pragma once

#include "Renderer.h"
#include "Graphic/Window.h"

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>


namespace ZE {

class SceneObject;
class RenderPass;
class DepthPass;
class DirectionalLightPass;
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
    void SetupFrame(TPtr<VulkanCommandBuffer> commandBuffer, TPtr<Frame> frame);
    virtual void RenderFrame(TPtr<VulkanCommandBuffer> commandBuffer, TPtr<Scene> scene, TPtr<Frame> frame) override;

private:
    VkFence _inFlightFence;

    TPtr<DepthPass> _depthPass;
    TPtr<DirectionalLightPass> _directionalLightPass;

    TPtrArr<RenderPass> _passes;
};

}