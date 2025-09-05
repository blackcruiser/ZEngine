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
    RenderSystem::Get().Initialize();
    InputSystem::Initialize();

    {
        RenderGraph* renderGraph = RenderSystem::Get().GetRenderGraph();

        scene->Load();

        Window* window = new Window(AppName, size);
        InputSystem::Get().AttachTo(window);


        window->CreateViewport(RenderSystem::Get().GetDevice());
        Viewport* viewport = window->GetViewport();


        RendererInterface* renderer = new ForwardRenderer();
        renderer->Init(renderGraph, viewport);
        while (!window->ShouldClose())
        {
            glfwPollEvents();

            RenderSystem::Get().InitializeResources(renderGraph);

            viewport->Advance(renderGraph);
            
            renderer->RenderFrame(renderGraph, viewport, scene);
            viewport->Present(renderGraph);

            RenderSystem::Get().DeleteGraphicResources();
        }


        renderer->Cleanup(renderGraph);
        RenderSystem::Get().CleanupResources(renderGraph);
        delete viewport;

        InputSystem::Get().DetachFrom(window);
        window->UnregisterInput(InputSystem::Get());
        delete window;

        scene->Unload();
    }

    RenderSystem::Get().Cleanup();
    InputSystem::Cleanup();
}

} // namespace ZE