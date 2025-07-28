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
    void Init(VulkanImage* colorRenderTarget, VulkanImage* depthRenderTarget);
    virtual void Draw(TPtr<RenderGraph>& commandBuffer, const TPtrArr<SceneObject>& objectsToRender) override;

private:
    TPtr<RenderTargets> renderTargets;
};

}