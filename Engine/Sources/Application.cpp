#include "Application.h"
#include "Graphic/VulkanDevice.h"
#include "Input/InputSystem.h"
#include "Render/Window.h"
#include "Render/RenderGraph.h"
#include "Render/RenderSystem.h"
#include "Render/Viewport.h"
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

    {
        TPtr<RenderGraph> renderGraph = std::make_shared<RenderGraph>();
        _renderer->Init(renderGraph, scene);
        RenderSystem::Get().GetDevice()->WaitIdle();
    }

    _window->CreateViewport(RenderSystem::Get().GetDevice());
    TPtr<Viewport> viewport = _window->GetViewport();

    while (!_window->ShouldClose())
    {
        glfwPollEvents();

        viewport->Advance();
        TPtr<RenderGraph> renderGraph = std::make_shared<RenderGraph>();
        _renderer->RenderFrame(renderGraph, viewport, scene);
        viewport->Present(renderGraph);
    }

    RenderSystem::Get().GetDevice()->WaitIdle();

    scene->Unload();
}

} // namespace ZE