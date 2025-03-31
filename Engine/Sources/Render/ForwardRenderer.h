#pragma once

#include "Renderer.h"
#include "Graphic/Window.h"

#include <glm/glm.hpp>


namespace ZE {

class SceneObject;
class DirectionalLightPass;
class DepthPass;
class RenderingCommandBuffer;
class RenderingContext;
class Surface;


class ForwardRenderer : public RendererInterface
{
public:
    ForwardRenderer();
    virtual ~ForwardRenderer();

    virtual void Init(TPtr<Scene> scene) override;
    TPtrArr<SceneObject> Prepare(TPtr<RenderingCommandBuffer> commandBuffer, TPtr<Scene> scene);
    void Draw(TPtr<RenderingCommandBuffer> commandBuffer, TPtr<Scene> scene);
    virtual void RenderFrame(TPtr<RenderingContext> renderingContext, TPtr<RenderingCommandBuffer> commandBuffer, TPtr<Scene> scene) override;

private:
    VkFence _inFlightFence;

    TPtr<DepthPass> _depthPass;
    TPtr<DirectionalLightPass> _directionalLightPass;
};

}