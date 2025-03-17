#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"


namespace ZE {

class SceneObject;
class CameraComponent;

class Scene
{
public:
    Scene();

    void AddObject(TPtr<SceneObject> object);
    void RemoveObject(TPtr<SceneObject> object);
    const TPtrArr<SceneObject>& GetObjects();

    void SetCamera(TPtr<CameraComponent> cameraComponent);
    TPtr<CameraComponent> GetCamera();

    void Load();
    void Unload();

private:
    TPtrArr<SceneObject> _objects;
    TPtr<CameraComponent> _cameraComponent;
};

}