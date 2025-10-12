#pragma once

#include "RenderPass.h"

namespace ZE {

class SceneResource;
class VulkanImage;

class DepthPass : public RenderPass
{
public:
    void Init(TPtr<VulkanImage> depthRenderTarget);
    virtual void Draw(RenderGraph* commandBuffer, SceneResource* sceneResource) override;
};

}