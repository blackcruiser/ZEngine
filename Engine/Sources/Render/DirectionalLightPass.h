#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"
#include "RenderPass.h"

namespace ZE {

class RenderingContext;
class RenderGraph;

class DirectionalLightPass : public RenderPass
{
public:
    virtual void Prepare(TPtr<Scene> scene) override;
    virtual void Draw(TPtr<RenderingContext>& renderingContext, TPtr<RenderGraph>& commandBuffer, const TPtrArr<SceneObject>& objectsToRender) override;
};

}