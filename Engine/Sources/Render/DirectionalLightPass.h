#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"
#include "RenderPass.h"

namespace ZE {

class SceneResource;
class RenderGraph;
class VulkanImage;
struct RenderTargets;

class DirectionalLightPass : public RenderPass
{
public:
    void Init(TPtr<VulkanImage> colorRenderTarget, TPtr<VulkanImage> depthRenderTarget);
    virtual void Draw(RenderGraph* commandBuffer, SceneResource* sceneResource) override;

private:
    TPtr<RenderTargets> renderTargets;
};

}