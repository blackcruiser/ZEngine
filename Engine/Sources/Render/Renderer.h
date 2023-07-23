#pragma once

#include "ZEDefine.h"
#include "ZEType.h"
#include "Graphic/VulkanDevice.h"
#include "Graphic/Window.h"

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>


namespace ZE {

class SceneObject;
class Scene;
class Window;
class DirectionalLightPass;
class DepthPass;
class VulkanCommandBuffer;


class RendererInterface
{
public:
    virtual void Init(TPtr<Scene> scene) = 0;
    virtual void RenderFrame(TPtr<Scene> scene, TPtr<Window> window) = 0;
};

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

} // namespace ZE