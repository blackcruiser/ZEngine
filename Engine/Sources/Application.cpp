#include "Application.h"
#include "Graphic/VulkanDevice.h"
#include "Graphic/Window.h"
#include "Graphic/VulkanBufferManager.h"
#include "Graphic/VulkanCommandBufferManager.h"
#include "Input/InputSystem.h"
#include "Render/RenderSystem.h"
#include "Render/ForwardRenderer.h"
#include "Render/Frame.h"
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

    TPtr<Frame> LastFrame;
    while (!_window->ShouldClose())
    {
        glfwPollEvents();


        TPtr<VulkanSwapchain> swapchain = _window->GetSwapchain();
        TPtr<Frame> frame = make_shared<Frame>(RenderSystem::Get().GetDevice(), swapchain);
        TPtr<VulkanCommandBuffer> commandBuffer = frame->GetCachedCommandBuffer();

        // vkWaitForFences(vkDevice, 1, &_inFlightFence, VK_TRUE, UINT64_MAX);
        // vkResetFences(vkDevice, 1, &_inFlightFence);

        _renderer->RenderFrame(commandBuffer, scene, frame);

        std::vector<VkSemaphore> waitSemaphores{frame->GetAvailableSemaphore()};
        std::vector<VkPipelineStageFlags> waitStages{VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

        VkSemaphore submitSemaphore = RenderSystem::Get().GetDevice()->CreateGraphicSemaphore();
        frame->PutSemaphore(submitSemaphore);

        TPtr<VulkanQueue> graphicQueue = RenderSystem::Get().GetQueue(VulkanQueue::EType::Graphic);
        graphicQueue->Submit(commandBuffer, waitSemaphores, waitStages, {submitSemaphore}, frame->GetFence());
        graphicQueue->Present(swapchain, {submitSemaphore});

        if (frame)
        {
            VkFence fence = frame->GetFence();
            vkWaitForFences(RenderSystem::Get().GetDevice()->GetRawDevice(), 1, &fence, VK_TRUE, UINT64_MAX);
        }
        LastFrame = frame;

        RenderSystem::Get().GetBufferManager()->Tick();
    }

    RenderSystem::Get().GetDevice()->WaitIdle();

    scene->Unload();
}

} // namespace ZE