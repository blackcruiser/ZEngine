#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"


namespace ZE {

class SceneResource;
class RenderGraph;
class RenderTargets;

class RenderPass
{
public:
    RenderPass();
    ~RenderPass();

    void Execute(RenderGraph* commandBuffer, SceneResource* sceneResource);
    virtual void Draw(RenderGraph* commandBuffer, SceneResource* sceneResource) = 0;

protected:
    TPtr<RenderTargets> renderTargets;
};

}