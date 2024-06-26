#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <functional>


namespace ZE {

class Window;

typedef std::function<void(const glm::vec2&)> MouseAction;
typedef std::function<void(int key, int action)> KeyboardAction;

class InputSystem
{
public:
    static void Initialize();
    static void Cleanup();
    static InputSystem& Get();

private:
    InputSystem();
    ~InputSystem();

public:
    void AttachTo(TPtr<Window> window);
    void DetachFrom(TPtr<Window> window);

    size_t RegisterMouseAction(MouseAction action);
    void UnregisterMouseAction(size_t key);

    size_t RegisterKeyboardAction(KeyboardAction action);
    void UnregisterKeyboardAction(size_t key);

    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

private:
    std::vector<MouseAction> _mouseActions;
    std::vector<KeyboardAction> _keyboardActions;

    TPtr<Window> _window;

    static InputSystem* _instance;
};

}