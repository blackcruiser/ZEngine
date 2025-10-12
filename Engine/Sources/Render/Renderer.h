#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"


namespace ZE {

class Scene;
class RenderGraph;
class Viewport;
class SceneResource;


class RendererInterface
{
public:
    virtual void Init(RenderGraph* renderGraph, Viewport* viewport) = 0;
    virtual void Cleanup(RenderGraph* renderGraph) = 0;
    virtual void RenderFrame(RenderGraph* renderGraph, Viewport* viewport, SceneResource* sceneResource) = 0;
};

} // namespace ZE