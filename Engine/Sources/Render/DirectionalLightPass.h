#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"
#include "RenderPass.h"

namespace ZE {


class RenderGraph;

class DirectionalLightPass : public RenderPass
{
public:
    virtual void Prepare(TPtr<Scene> scene) override;
    virtual void Draw(TPtr<RenderGraph>& commandBuffer, const TPtrArr<SceneObject>& objectsToRender) override;
};

}