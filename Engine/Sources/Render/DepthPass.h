#pragma once

#include "RenderPass.h"

namespace ZE {

class VulkanImage;
struct RenderTargets;

class DepthPass : public RenderPass
{
public:
    void Init(VulkanImage* depthRenderTarget);
    virtual void Draw(TPtr<RenderGraph>& commandBuffer, const TPtrArr<SceneObject>& objectsToRender) override;

private:
    TPtr<RenderTargets> renderTargets;
};

}