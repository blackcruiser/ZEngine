#pragma once

#include "RenderPass.h"

namespace ZE {

class DepthPass : public RenderPass
{
public:
    void Setup(TPtr<VulkanImageView> Depth);

    virtual RenderTargets GetRenderTargets() override;

    virtual void Draw(TPtrArr<SceneObject> objectsToRender, TPtr<VulkanCommandBuffer> commandBuffer) override;

private:
    TPtr<VulkanImageView> m_depth;
};

}