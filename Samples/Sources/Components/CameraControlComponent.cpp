#include "CameraControlComponent.h"

#include "Scene/SceneObject.h"
#include "Scene/SceneComponent.h"
#include "Scene/TransformComponent.h"

#include <glm/ext/matrix_transform.hpp>

#include <functional>


CameraControlComponent::CameraControlComponent() : cachedMousePosition(glm::zero<glm::vec2>())
{
}

CameraControlComponent::~CameraControlComponent()
{
}


void CameraControlComponent::OnAttached()
{
    ZE::MouseAction mouseAction = std::bind(&CameraControlComponent::OnMouseInput, this, std::placeholders::_1);
    _mouseActionKey = ZE::InputSystem::Get().RegisterMouseAction(mouseAction);

    ZE::KeyboardAction keyboardAction = std::bind(&CameraControlComponent::OnKeyboardInput, this, std::placeholders::_1, std::placeholders::_2);
    _keyboardActionKey = ZE::InputSystem::Get().RegisterKeyboardAction(keyboardAction);
}

void CameraControlComponent::OnDetached()
{
    ZE::InputSystem::Get().UnregisterMouseAction(_mouseActionKey);
    ZE::InputSystem::Get().UnregisterKeyboardAction(_keyboardActionKey);
}

void CameraControlComponent::OnMouseInput(const glm::vec2& position)
{
    ZE::TPtr<ZE::TransformComponent> transformComponent = GetObject()->GetComponent<ZE::TransformComponent>();
    if (transformComponent == nullptr)
        return;

    if (cachedMousePosition != glm::zero<glm::vec2>())
    {
        glm::vec2 delta = position - cachedMousePosition;
        glm::vec2 rotate = delta * 0.1f;

        glm::mat4 transform = transformComponent->GetTransform();

        transform = glm::rotate(transform, glm::radians(rotate.y), glm::vec3(1.0f, 0.0f, 0.0f));
        transform = glm::rotate(transform, glm::radians(rotate.x), glm::vec3(0.0f, -1.0f, 0.0f));

        transformComponent->SetTransform(transform);
    }

    cachedMousePosition = position;
}

void CameraControlComponent::OnKeyboardInput(int key, int action)
{
    ZE::TPtr<ZE::TransformComponent> transformComponent = GetObject()->GetComponent<ZE::TransformComponent>();
    if (transformComponent == nullptr)
        return;

    if (action == GLFW_PRESS || action == GLFW_REPEAT)
        cachedPressedKey.insert(key);
    else if (action == GLFW_RELEASE)
        cachedPressedKey.erase(key);

    glm::vec3 translation{ 0 };
    for (const int key : cachedPressedKey)
    {
        glm::vec3 delta{ 0 };

        switch (key)
        {
        case GLFW_KEY_W:
            delta = glm::vec3(0.0f, -1.0f, 0.0f);
            break;
        case GLFW_KEY_S:
            delta = glm::vec3(0.0f, 1.0f, 0.0f);
            break;
        case GLFW_KEY_A:
            delta = glm::vec3(-1.0f, 0.0f, 0.0f);
            break;
        case GLFW_KEY_D:
            delta = glm::vec3(1.0f, 0.0f, 0.0f);
            break;
        }

        translation = translation + delta * 1.0f;
    }

    glm::mat4 transform = transformComponent->GetTransform();
    transform = glm::translate(transform, translation);
    transformComponent->SetTransform(transform);
}