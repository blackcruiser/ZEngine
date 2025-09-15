#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"
#include "BaseResource.h"


namespace ZE {

class SceneObject;
class CameraComponent;
class SceneResource;
class ObjectResource;

class Scene : public BaseResource
{
public:
    Scene();

    void AddObject(TPtr<SceneObject> object);
    void RemoveObject(TPtr<SceneObject> object);
    const TPtrArr<SceneObject>& GetObjects();

    void SetCamera(TPtr<CameraComponent> cameraComponent);
    TPtr<CameraComponent> GetCamera();

    virtual void Load() override;
    virtual void PostLoad() override;

    virtual void Unload() override;

    SceneResource* CreateRenderResource();

private:
    TPtrArr<SceneObject> _objects;
    TPtr<CameraComponent> _cameraComponent;
};

class SceneResource
{
public:
    SceneResource();

    void AddObjectResource(ObjectResource* resource);
    std::vector<ObjectResource*>& GetObjectResources();

private:
    std::vector<ObjectResource*> _objectResources;
};

}