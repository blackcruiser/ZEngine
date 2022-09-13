#pragma once

#include "CoreDefines.h"


namespace ZE {

class Scene;


class RendererInterface
{
public:
    virtual void Init(TPtr<Scene> scene) = 0;
    virtual void RenderFrame(TPtr<Scene> scene) = 0;
};

} // namespace ZE