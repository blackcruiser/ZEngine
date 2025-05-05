#pragma once

#include "Renderer.h"
#include "Window.h"

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

    virtual void Init(TPtr<RenderGraph> renderGraph, TPtr<Scene> scene) override;
    TPtrArr<SceneObject> Prepare(TPtr<RenderGraph> renderGraph, TPtr<Scene> scene);
    void Draw(TPtr<RenderGraph> commandBuffer, TPtr<Scene> scene);
    virtual void RenderFrame(TPtr<RenderGraph> commandBuffer, Viewport* viewport, TPtr<Scene> scene) override;

private:
    TPtr<DepthPass> _depthPass;
    TPtr<DirectionalLightPass> _directionalLightPass;
};

}