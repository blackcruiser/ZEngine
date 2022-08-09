#pragma once

#include "CoreDefines.h"


namespace TE {

class Scene;


class RendererInterface
{
public:
    virtual void RenderFrame(TPtr<Scene> scene) = 0;
};

} // namespace TE