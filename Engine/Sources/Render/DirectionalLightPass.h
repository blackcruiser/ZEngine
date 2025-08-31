#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"
#include "RenderPass.h"

namespace ZE {


class RenderGraph;
class VulkanImage;
struct RenderTargets;

class DirectionalLightPass : public RenderPass
{
public:
    void Init(TPtr<VulkanImage> colorRenderTarget, TPtr<VulkanImage> depthRenderTarget);
    virtual void Draw(RenderGraph*& commandBuffer, const TPtrArr<SceneObject>& objectsToRender) override;

private:
    TPtr<RenderTargets> renderTargets;
};

}