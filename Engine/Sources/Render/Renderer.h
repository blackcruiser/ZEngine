#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"


namespace ZE {

class Scene;
class Frame;
class RenderingContext;
class RenderingCommandBuffer;


class RendererInterface
{
public:
    virtual void Init(TPtr<Scene> scene) = 0;
    virtual void RenderFrame(TPtr<RenderingContext> renderingContext, TPtr<RenderingCommandBuffer> commandBuffer, TPtr<Scene> scene) = 0;
};

} // namespace ZE