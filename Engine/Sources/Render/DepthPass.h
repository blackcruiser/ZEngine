#pragma once

#include "RenderPass.h"

namespace ZE {

class DepthPass : public RenderPass
{
public:
    virtual void Prepare(TPtr<Scene> scene) override;
    virtual void Draw(TPtr<RenderingContext>& renderingContext, TPtr<RenderGraph>& commandBuffer, const TPtrArr<SceneObject>& objectsToRender) override;
};

}