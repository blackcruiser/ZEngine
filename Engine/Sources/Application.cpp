#include "Application.h"
#include "Graphic/Window.h"
#include "Graphic/VulkanDevice.h"
#include "Input/InputSystem.h"
#include "Render/RenderingContext.h"
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

    TPtr<RenderingContext> renderingContext = RenderSystem::Get().CreateRenderingContext();
    renderingContext->Initialize();

    {
        TPtr<RenderGraph> renderGraph = std::make_shared<RenderGraph>(renderingContext);
        _renderer->Init(renderingContext, renderGraph, scene);
        RenderSystem::Get().GetDevice()->WaitIdle();
    }

    TPtr<Viewport> viewport = std::make_shared<Viewport>(_window->GetFramebufferSize(), _window->GetSwapchain());

    while (!_window->ShouldClose())
    {
        glfwPollEvents();

        viewport->Advance();
        TPtr<RenderGraph> renderGraph = std::make_shared<RenderGraph>(renderingContext);
        _renderer->RenderFrame(renderingContext, renderGraph, viewport, scene);
        viewport->Present(renderingContext, renderGraph);
    }

    RenderSystem::Get().GetDevice()->WaitIdle();

    scene->Unload();
}

} // namespace ZE