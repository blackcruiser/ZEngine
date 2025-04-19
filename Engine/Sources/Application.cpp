#include "Application.h"
#include "Graphic/VulkanDevice.h"
#include "Input/InputSystem.h"
#include "Render/RenderResource.h"
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
{


}

Application::~Application()
{

}

void Application::Run(TPtr<Scene> scene)
{
    scene->Load();

    RenderSystem::Get().Initialize();
    InputSystem::Initialize();

    TPtr<Window> window = std::make_shared<Window>(AppName, size);
    InputSystem::Get().AttachTo(window);


    window->CreateViewport(RenderSystem::Get().GetDevice());
    TPtr<Viewport> viewport = window->GetViewport();

    TPtr<RendererInterface> renderer = std::make_shared<ForwardRenderer>();
    while (!window->ShouldClose())
    {
        glfwPollEvents();

        viewport->Advance();
        TPtr<RenderGraph> renderGraph = std::make_shared<RenderGraph>();
        renderer->RenderFrame(renderGraph, viewport, scene);
        viewport->Present(renderGraph);
    }

    TPtr<VulkanQueue> graphicQueue = RenderSystem::Get().GetQueue(VulkanQueue::EType::Graphic);
    graphicQueue->WaitIdle();
    graphicQueue.reset();

    renderer.reset();
    viewport.reset();

    InputSystem::Get().DetachFrom(window);
    window->UnregisterInput(InputSystem::Get());
    window.reset();

    RenderSystem::Get().Cleanup();
    InputSystem::Cleanup();

    scene->Unload();
}

} // namespace ZE