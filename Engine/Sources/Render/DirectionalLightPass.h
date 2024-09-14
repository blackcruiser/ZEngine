#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"
#include "RenderPass.h"

namespace ZE {

class FRenderTargetBindings;

class DirectionalLightPass : public RenderPass
{
public:
    void Setup(TPtr<VulkanImageView> color, TPtr<VulkanImageView> depth);

    virtual RenderTargets GetRenderTargets() override;

    virtual void Draw(TPtrArr<SceneObject> objectsToRender, TPtr<VulkanCommandBuffer> commandBuffer) override;

private:
    TPtr<VulkanImageView> m_color;
    TPtr<VulkanImageView> m_depth;
};

}