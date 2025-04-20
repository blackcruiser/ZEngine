#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"

#include <memory>


namespace ZE {

class Scene;

class Application
{
public:
    Application();
    ~Application();

    void Run(TPtr<Scene> scene);
};

}