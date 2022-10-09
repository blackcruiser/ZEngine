#include "InputSystem.h"
#include "Graphic/Window.h"

#include <iostream>
#include <format>


namespace ZE {

InputSystem* InputSystem::_instance = nullptr;

void InputSystem::Initialize()
{
    assert(_instance == nullptr);

    if (_instance != nullptr)
        return;

    _instance = new InputSystem();
}

void InputSystem::Cleanup()
{
    assert(_instance);

    if (_instance == nullptr)
        return;

    delete _instance;
    _instance = nullptr;
}

InputSystem& InputSystem::Get()
{
    static InputSystem instance;

    return instance;
}


InputSystem::InputSystem()
{
}

InputSystem::~InputSystem()
{
}

void InputSystem::AttachTo(TPtr<Window> window)
{
    _window = window;
    _window->RegisterInput(InputSystem::Get());
}

void InputSystem::DetachFrom(TPtr<Window> window)
{
    _window->UnregisterInput(InputSystem::Get());
    _window = nullptr;
}

size_t InputSystem::RegisterMouseAction(MouseAction action)
{
    _mouseActions.push_back(action);
    return _mouseActions.size() - 1;
}

void InputSystem::UnregisterMouseAction(size_t key)
{
    _mouseActions.erase(_mouseActions.begin() + key);
}

size_t InputSystem::RegisterKeyboardAction(KeyboardAction action)
{
    _keyboardActions.push_back(action);
    return _keyboardActions.size() - 1;
}

void InputSystem::UnregisterKeyboardAction(size_t key)
{
    _keyboardActions.erase(_keyboardActions.begin() + key);
}

void InputSystem::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    glm::vec2 position{xpos, ypos};
    for (const MouseAction& mouseAction : Get()._mouseActions)
        mouseAction(position);
}

void InputSystem::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    for (const KeyboardAction& keyboardAction : Get()._keyboardActions)
        keyboardAction(key, action);
}

} // namespace ZE