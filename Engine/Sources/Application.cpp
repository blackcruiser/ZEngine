#include "Application.h"
#include "Graphic/Window.h"
#include "Graphic/VulkanBufferManager.h"
#include "Input/InputSystem.h"
#include "Render/RenderSystem.h"
#include "Render/ForwardRenderer.h"
#include "Scene/Scene.h"

#include <stdexcept>
#include <string>


namespace ZE {

const std::string AppName("ZEngine");
const glm::ivec2 size{800, 800};

Application::Application()
    : _renderer(nullptr)
{
    RenderSystem::Initialize();
    InputSystem::Initialize();

    _window = std::make_shared<Window>(AppName, size);
    _window->CreateSurfaceAndSwapchain(RenderSystem::Get().GetDevice());

    InputSystem::Get().AttachTo(_window);

    _renderer = std::make_shared<ForwardRenderer>();
}

Application::~Application()
{
    _renderer.reset();

    InputSystem::Get().DetachFrom(_window);

    _window->UnregisterInput(InputSystem::Get());
    _window.reset();

    InputSystem::Cleanup();
    RenderSystem::Cleanup();
}

void Application::Run(TPtr<Scene> scene)
{
    scene->Load();

    _renderer->Init(scene);

    while (!_window->ShouldClose())
    {
        glfwPollEvents();

        _renderer->RenderFrame(scene, _window);

        RenderSystem::Get().GetBufferManager()->Tick();
    }

    RenderSystem::Get().GetDevice()->WaitIdle();

    scene->Unload();
}

} // namespace ZE