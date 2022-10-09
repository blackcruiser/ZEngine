#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"


namespace ZE {

class Scene;
class Window;


class RendererInterface
{
public:
    virtual void Init(TPtr<Scene> scene) = 0;
    virtual void RenderFrame(TPtr<Scene> scene, TPtr<Window> window) = 0;
};

} // namespace ZE