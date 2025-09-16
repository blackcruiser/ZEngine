#pragma once

#include "Renderer.h"
#include "Window.h"
#include "Core/ReferenceCountObject.h"

#include <glm/glm.hpp>


namespace ZE {

class SceneObject;
class DirectionalLightPass;
class DepthPass;
class RenderGraph;
class Viewport;


class ForwardRenderer : public RendererInterface
{
public:
    ForwardRenderer();
    virtual ~ForwardRenderer();

    virtual void Init(RenderGraph* renderGraph, Viewport* viewport) override;
    virtual void Cleanup(RenderGraph* renderGraph) override;

    void Draw(RenderGraph* commandBuffer, TPtr<Scene> scene);
    virtual void RenderFrame(RenderGraph* commandBuffer, Viewport* viewport, SceneResource* sceneResource) override;

private:
    TPtr<VulkanImage> _depthRenderTarget;
    TPtr<DepthPass> _depthPass;
    TPtr<DirectionalLightPass> _directionalLightPass;
};

}