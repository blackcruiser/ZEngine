#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"

#include <optional>

namespace ZE {

class SceneObject;
class Scene;
struct RenderTargets;
class RenderGraph;

class RenderPass
{
public:
    RenderPass();
    ~RenderPass();

    void Execute(RenderGraph*& commandBuffer, const TPtrArr<SceneObject>& objectsToRender);
    virtual void Draw(RenderGraph*& commandBuffer, const TPtrArr<SceneObject>& objectsToRender) = 0;
};

}