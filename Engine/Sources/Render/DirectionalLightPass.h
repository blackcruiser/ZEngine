#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"
#include "RenderPass.h"

namespace ZE {

class FRenderTargetBindings;

class DirectionalLightPass : public RenderPass
{
public:
    virtual void Prepare(TPtr<Scene> scene) override;
    virtual void Draw(TPtrArr<SceneObject> objectsToRender, TPtr<VulkanCommandBuffer> commandBuffer, TPtr<RenderTargets> renderTargets) override;
};

}