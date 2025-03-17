#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"


namespace ZE {

enum class EComponentType : int
{
    Invalid = 0,
    Camera,
    Transform,
    Mesh,
    Script,
};

class SceneObject;

class SceneComponent
{
public:
    SceneComponent(const EComponentType type);
    ~SceneComponent();

    const EComponentType GetType() const;

    void SetObject(TPtr<SceneObject> object);
    TPtr<SceneObject> GetObject();

    // liftCycle
    virtual void Load(){};
    virtual void Unload(){};

    virtual void OnAttached(){};
    virtual void Update(float deltaTime){};
    virtual void OnDetached(){};

private:
    EComponentType _type;
    TWeakPtr<SceneObject> _sceneObject;
};

} // namespace ZE