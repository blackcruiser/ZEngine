#pragma once

#include "CoreDefines.h"


namespace TE {

enum class EComponentType : int
{
    Invalid = 0,
    Camera,
    Transform,
    Mesh,
    Material,
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
    virtual void Load() {};
    
    virtual void OnAttached() {};
    virtual void Update(float deltaTime) {};
    virtual void OnDetached() {};

private:
    EComponentType _type;
    TWeakPtr<SceneObject> _sceneObject;
};

}