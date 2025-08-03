#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"


namespace ZE {

class Scene;
class RenderGraph;
class Viewport;


class RendererInterface
{
public:
    virtual void Init(TPtr<RenderGraph> renderGraph, Viewport* viewport) = 0;
    virtual void Cleanup(TPtr<RenderGraph> renderGraph) = 0;;
    virtual void RenderFrame(TPtr<RenderGraph> renderGraph, Viewport* viewport, TPtr<Scene> scene) = 0;
};

} // namespace ZE