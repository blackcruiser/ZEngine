#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"

#include <optional>

namespace ZE {

class SceneObject;
class Scene;
struct RenderTargets;
class RenderingContext;
class RenderGraph;

class RenderPass
{
public:
    RenderPass();
    ~RenderPass();

    virtual void Prepare(TPtr<Scene> scene) = 0;
    void Execute(TPtr<RenderingContext>& renderingContext, TPtr<RenderGraph>& commandBuffer, const TPtrArr<SceneObject>& objectsToRender);
    virtual void Draw(TPtr<RenderingContext>& renderingContext, TPtr<RenderGraph>& commandBuffer, const TPtrArr<SceneObject>& objectsToRender) = 0;
};

}