#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"
#include "SceneComponent.h"

#include <glm/glm.hpp>

#include <memory>


namespace ZE {

class SceneObject : public std::enable_shared_from_this<SceneObject>
{
public:
    SceneObject();
    ~SceneObject();

    template <typename T>
    requires std::is_convertible<T, SceneComponent>::value
        void AddComponent(TPtr<T> component)
    {
        _componentArr.push_back(component);

        component->SetObject(shared_from_this());
    }

    template <typename T>
    requires std::is_convertible<T, SceneComponent>::value
        TPtr<T> GetComponent()
    {
        for (const TPtr<SceneComponent>& component : _componentArr)
        {
            TPtr<T> result = std::dynamic_pointer_cast<T>(component);
            if (result != nullptr)
                return result;
        }

        return nullptr;
    }

    template <typename T>
    requires std::is_convertible<T, SceneComponent>::value
        TPtrArr<T> GetComponents()
    {
        TPtrArr<T> components;

        for (const TPtr<SceneComponent>& component : _componentArr)
        {
            TPtr<T> result = std::dynamic_pointer_cast<T>(component);
            if (result != nullptr)
                components.push_back(result);
        }

        return components;
    }

private:
    TPtrArr<SceneComponent> _componentArr;
};

}
