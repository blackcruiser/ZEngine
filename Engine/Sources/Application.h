#pragma once

#include "ZEDefine.h"
#include "ZEType.h"

#include <memory>


namespace ZE {

class Window;
class RendererInterface;
class Scene;

class Application
{
public:
    Application();
    ~Application();

    void Run(TPtr<Scene> scene);

private:
    TPtr<Window> _window;
    TPtr<RendererInterface> _renderer;
};

}