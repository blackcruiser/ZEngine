#include "Scene.h"
#include "CameraComponent.h"
#include "SceneObject.h"
#include "ScriptComponent.h"

#include <stdexcept>


namespace TE {

Scene::Scene()
{
}

void Scene::AddObject(TPtr<SceneObject> object)
{
    TPtrArr<ScriptComponent> scriptComponents = object->GetComponents<ScriptComponent>();
    for (const TPtr<ScriptComponent>& scriptComponent : scriptComponents)
        scriptComponent->OnAttached();

    _objects.push_back(object);
}

void Scene::RemoveObject(TPtr<SceneObject> object)
{
    _objects.erase(std::remove(_objects.begin(), _objects.end(), object));

    TPtrArr<ScriptComponent> scriptComponents = object->GetComponents<ScriptComponent>();
    for (const TPtr<ScriptComponent>& scriptComponent : scriptComponents)
        scriptComponent->OnDetached();
}

const TPtrArr<SceneObject>& Scene::GetObjects()
{
    return _objects;
}

void Scene::SetCamera(TPtr<CameraComponent> cameraComponent)
{
    if (cameraComponent == nullptr)
        throw std::runtime_error("cameraComponent is null or object is null");

    TWeakPtr<SceneObject> object = cameraComponent->GetObject();
    if (_cameraComponent != nullptr)
        RemoveObject(object.lock());
    AddObject(object.lock());

    _cameraComponent = cameraComponent;
}

TPtr<CameraComponent> Scene::GetCamera()
{
    return _cameraComponent;
}

void Scene::Load()
{
    for (TPtr<SceneObject>& object : _objects)
    {
        TPtrArr<SceneComponent> components = object->GetComponents<SceneComponent>();
        for (TPtr<SceneComponent>& component : components)
        {
            component->Load();
        }
    }
}

void Scene::Unload()
{
    for (TPtr<SceneObject>& object : _objects)
    {
        TPtrArr<SceneComponent> components = object->GetComponents<SceneComponent>();
        for (TPtr<SceneComponent>& component : components)
        {
            component->Unload();
        }
    }
}

} // namespace TE