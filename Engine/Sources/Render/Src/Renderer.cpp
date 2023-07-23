#include "Renderer.h"
#include "Graphic/VulkanSurface.h"
#include "Graphic/VulkanBuffer.h"
#include "Graphic/VulkanImage.h"
#include "Graphic/VulkanImageView.h"
#include "Graphic/VulkanShader.h"
#include "Graphic/VulkanCommandBuffer.h"
#include "Graphic/VulkanCommandBufferManager.h"
#include "Graphic/VulkanDescriptorPool.h"
#include "Graphic/VulkanDescriptorSet.h"
#include "Graphic/VulkanPipelineLayout.h"
#include "Graphic/VulkanPipeline.h"
#include "Graphic/VulkanSwapchain.h"
#include "Graphic/VulkanQueue.h"
#include "RenderSystem.h"
#include "Material.h"
#include "Mesh.h"
#include "DirectionalLightPass.h"
#include "DepthPass.h"
#include "Resource/MaterialResource.h"
#include "Resource/MeshResource.h"
#include "Scene/Scene.h"
#include "Scene/SceneObject.h"
#include "Scene/TransformComponent.h"
#include "Scene/CameraComponent.h"
#include "Scene/MeshComponent.h"

#include <algorithm>


namespace ZE {

ForwardRenderer::ForwardRenderer()
{
    _imageAvailableSemaphore = RenderSystem::Get().GetDevice()->CreateGraphicSemaphore();
    _renderFinishedSemaphore = RenderSystem::Get().GetDevice()->CreateGraphicSemaphore();

    _inFlightFence = RenderSystem::Get().GetDevice()->CreateFence(true);

    _depthPass = std::make_shared<DepthPass>();
    _directionalLightPass = std::make_shared<DirectionalLightPass>();
}

ForwardRenderer::~ForwardRenderer()
{
    RenderSystem::Get().GetDevice()->DestroyGraphicSemaphore(_imageAvailableSemaphore);
    RenderSystem::Get().GetDevice()->DestroyGraphicSemaphore(_renderFinishedSemaphore);
    RenderSystem::Get().GetDevice()->DestroyFence(_inFlightFence);
}

void ForwardRenderer::Init(TPtr<Scene> scene)
{
    TPtr<VulkanCommandBuffer> commandBuffer = RenderSystem::Get().GetCommandBufferManager()->GetCommandBuffer(VulkanQueue::EType::Graphic);

    commandBuffer->Begin();

    const TPtrArr<SceneObject>& objects = scene->GetObjects();

    for (TPtr<SceneObject> object : objects)
    {
        TPtr<MeshComponent> meshComponent = object->GetComponent<MeshComponent>();
        if (meshComponent == nullptr)
            continue;

        TPtr<MeshResource> meshResource = meshComponent->GetMesh();
        if (meshResource != nullptr)
        {
            TPtr<Mesh> mesh = std::make_shared<Mesh>(meshResource);
            mesh->CreateVertexBuffer(commandBuffer);
            mesh->CreateIndexBuffer(commandBuffer);
            meshResource->SetMesh(mesh);
        }

        TPtr<MaterialResource> materialResource = meshComponent->GetMaterial(0);
        if (materialResource != nullptr)
        {
            TPtr<Material> material = std::make_shared<Material>(materialResource);
            materialResource->SetMaterial(material);

            for (int i = 0; i < static_cast<int>(PassType::PassCount); i++)
            {
                PassType passType = static_cast<PassType>(i);

                TPtr<PassResource> passResource = materialResource->GetPass(passType);
                if (passResource != nullptr)
                {
                    TPtr<Pass> pass = std::make_shared<Pass>(passResource);
                    material->SetPass(passType, pass);
                    pass->BuildRenderResource(commandBuffer);
                }
            }
        }
    }

    commandBuffer->End();

    TPtr<VulkanQueue> transferQueue = RenderSystem::Get().GetQueue(VulkanQueue::EType::Graphic);
    transferQueue->Submit(commandBuffer, {}, {}, {}, VK_NULL_HANDLE);
    transferQueue->WaitIdle();
}

TPtrArr<SceneObject> ForwardRenderer::Prepare(TPtr<VulkanCommandBuffer> commandBuffer, TPtr<Scene> scene)
{
    //Filter Objects
    const TPtrArr<SceneObject>& allObjects = scene->GetObjects();
    TPtrArr<SceneObject> objectsToRender;
    std::copy_if(allObjects.begin(), allObjects.end(), std::back_inserter(objectsToRender), [](TPtr<SceneObject> object) {
        TPtr<MeshComponent> meshComponent = object->GetComponent<MeshComponent>();
        if (meshComponent == nullptr)
            return false;

        TPtr<MeshResource> meshResource = meshComponent->GetMesh();
        TPtr<MaterialResource> materialResource = meshComponent->GetMaterial(0);
        if (meshResource == nullptr || materialResource == nullptr)
            return false;

        TPtr<Mesh> mesh = meshResource->GetMesh();
        TPtr<Material> material = materialResource->GetMaterial();
        if (mesh == nullptr || material == nullptr)
            return false;

        return true;
    });


    // Update Uniform Buffer
    {
        TPtr<CameraComponent> cameraComponent = scene->GetCamera();
        glm::mat4x4 VP = cameraComponent->GetProjectMatrix() * cameraComponent->GetViewMatrix();

        for (TPtr<SceneObject>& object : objectsToRender)
        {
            TPtr<MeshComponent> meshComponent = object->GetComponent<MeshComponent>();

            TPtr<MeshResource> meshResource = meshComponent->GetMesh();
            TPtr<MaterialResource> materialResource = meshComponent->GetMaterial(0);

            TPtr<Mesh> mesh = meshResource->GetMesh();
            TPtr<Material> material = materialResource->GetMaterial();

            TPtr<TransformComponent> transformComponent = object->GetComponent<TransformComponent>();
            glm::mat4x4 MVP = VP * transformComponent->GetTransform();

            for (int i = 0; i < static_cast<int>(PassType::PassCount); i++)
            {
                PassType passType = static_cast<PassType>(i);

                TPtr<Pass> pass = material->GetPass(passType);
                if (pass)
                {
                    // Update Global DescriptorSet
                    pass->UpdateUniformBuffer(commandBuffer, MVP);
                }
            }
        }
    }

    return objectsToRender;
}

void ForwardRenderer::RenderFrame(TPtr<Scene> scene, TPtr<Window> window)
{
    TPtr<VulkanDevice> device = RenderSystem::Get().GetDevice();
    VkDevice vkDevice = device->GetRawDevice();

    TPtr<VulkanSurface> surface = window->GetSurface();
    VkSurfaceFormatKHR surfaceFormat = surface->GetSurfaceFormat();
    VkExtent2D extent = surface->GetExtent();

    TPtr<VulkanQueue> graphicQueue = RenderSystem::Get().GetQueue(VulkanQueue::EType::Graphic);
    TPtr<VulkanCommandBuffer> commandBuffer = RenderSystem::Get().GetCommandBufferManager()->GetCommandBuffer(VulkanQueue::EType::Graphic);
    VkCommandBuffer vkCommandBuffer = commandBuffer->GetRawCommandBuffer();

    vkWaitForFences(vkDevice, 1, &_inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(vkDevice, 1, &_inFlightFence);

    TPtr<VulkanSwapchain> swapchain = window->GetSwapchain();
    TPtr<VulkanImage> sceneImage = swapchain->AcquireNextImage(UINT64_MAX, _imageAvailableSemaphore, VK_NULL_HANDLE);  

    commandBuffer->Begin();

    TPtrArr<SceneObject> objectsToRender = Prepare(commandBuffer, scene);

    //Depth Pass
    TPtr<VulkanImage> depthImage = std::make_shared<VulkanImage>(device, VkExtent3D{extent.width, extent.height, 1}, VkFormat::VK_FORMAT_D32_SFLOAT, VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
    TPtr<VulkanImageView> depthImageView = std::make_shared<VulkanImageView>(depthImage, VkFormat::VK_FORMAT_D32_SFLOAT, VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT);
    TPtr<RenderTargets> depthRenderTargets = std::make_shared<RenderTargets>();
    depthRenderTargets->depthStencil = RenderTargetBinding{depthImageView, ERenderTargetLoadAction::None};
    _depthPass->Execute(objectsToRender, commandBuffer, depthRenderTargets);

    //Light Pass
    TPtr<RenderTargets> sceneRenderTargets = std::make_shared<RenderTargets>();
    TPtrArr<VulkanImage> framebufferImageAttachments{sceneImage};
    std::transform(framebufferImageAttachments.begin(), framebufferImageAttachments.end(), std::back_inserter(sceneRenderTargets->colors), [](TPtr<VulkanImage> image) {
        return RenderTargetBinding(std::make_shared<VulkanImageView>(image), ERenderTargetLoadAction::Clear);
    });
    _directionalLightPass->Execute(objectsToRender, commandBuffer, sceneRenderTargets);

    commandBuffer->End();

    std::vector<VkSemaphore> waitSemaphores{_imageAvailableSemaphore};
    std::vector<VkPipelineStageFlags> waitStages{VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    // std::vector<VkSemaphore> signalSemaphores{_renderFinishedSemaphore};
    std::vector<VkSemaphore> signalSemaphores{};

    graphicQueue->Submit(commandBuffer, waitSemaphores, waitStages, signalSemaphores, _inFlightFence);

    graphicQueue->Present(swapchain, {});
    graphicQueue->WaitIdle();
}


} // namespace ZE