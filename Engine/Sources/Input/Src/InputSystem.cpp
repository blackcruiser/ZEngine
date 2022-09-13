#include "InputSystem.h"

#include <iostream>
#include <format>


namespace ZE {

InputSystem& InputSystem::GetInstance()
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
    glm::vec2 position{ xpos, ypos };
    for (const MouseAction& mouseAction : GetInstance()._mouseActions)
        mouseAction(position);
}

void InputSystem::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    for (const KeyboardAction& keyboardAction : GetInstance()._keyboardActions)
        keyboardAction(key, action);
}

}