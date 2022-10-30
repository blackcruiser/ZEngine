#include "ForwardRenderer.h"
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
#include "Graphic/VulkanRenderPass.h"
#include "Graphic/VulkanFramebuffer.h"
#include "Graphic/VulkanQueue.h"
#include "RenderSystem.h"
#include "Material.h"
#include "Mesh.h"
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

            for (int i = 0; i < static_cast<int>(EPassType::PassCount); i++)
            {
                EPassType passType = static_cast<EPassType>(i);

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

void ForwardRenderer::RenderFrame(TPtr<Scene> scene, TPtr<Window> window)
{
    TPtr<VulkanDevice> device = RenderSystem::Get().GetDevice();
    VkDevice vkDevice = device->GetRawDevice();

    // Begin render
    vkWaitForFences(vkDevice, 1, &_inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(vkDevice, 1, &_inFlightFence);

    TPtr<VulkanQueue> graphicQueue = RenderSystem::Get().GetQueue(VulkanQueue::EType::Graphic);
    TPtr<VulkanCommandBuffer> commandBuffer = RenderSystem::Get().GetCommandBufferManager()->GetCommandBuffer(VulkanQueue::EType::Graphic);

    commandBuffer->Begin();

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

            for (int i = 0; i < static_cast<int>(EPassType::PassCount); i++)
            {
                EPassType passType = static_cast<EPassType>(i);

                TPtr<Pass> pass = material->GetPass(passType);
                if (pass)
                {
                    // Update Global DescriptorSet
                    pass->UpdateUniformBuffer(commandBuffer, MVP);
                }
            }
        }
    }


    TPtr<VulkanSurface> surface = window->GetSurface();
    VkSurfaceFormatKHR surfaceFormat = surface->GetSurfaceFormat();
    VkExtent2D extent = surface->GetExtent();

    TPtr<VulkanSwapchain> swapchain = window->GetSwapchain();


    TPtrUnorderedSet<VulkanGraphicPipeline> usedPipelineSet;
    TPtr<VulkanRenderPass> renderPass = std::make_shared<VulkanRenderPass>(device, surfaceFormat.format);

    TPtr<VulkanImage> sceneImage = swapchain->AcquireNextImage(UINT64_MAX, _imageAvailableSemaphore, VK_NULL_HANDLE);
    TPtrArr<VulkanImage> framebufferImageAttachments{sceneImage};
    TPtrArr<VulkanImageView> framebufferImageViewAttachments;
    std::transform(framebufferImageAttachments.begin(), framebufferImageAttachments.end(), std::back_inserter(framebufferImageViewAttachments), [](TPtr<VulkanImage> image) {
        return std::make_shared<VulkanImageView>(image);
    });

    TPtr<VulkanFramebuffer> framebuffer = std::make_shared<VulkanFramebuffer>(device, renderPass, framebufferImageViewAttachments, extent);
    VkCommandBuffer vkCommandBuffer = commandBuffer->GetRawCommandBuffer();


    commandBuffer->BeginRenderPass(renderPass, framebuffer, {{0, 0}, extent}, {0, 0, 0, 1});


    // Opaque Pass
    {
        EPassType passType = EPassType::BasePass;
        for (TPtr<SceneObject>& object : objectsToRender)
        {
            TPtr<MeshComponent> meshComponent = object->GetComponent<MeshComponent>();

            TPtr<MeshResource> meshResource = meshComponent->GetMesh();
            TPtr<MaterialResource> materialResource = meshComponent->GetMaterial(0);

            TPtr<Mesh> mesh = meshResource->GetMesh();
            TPtr<Material> material = materialResource->GetMaterial();
            TPtr<Pass> pass = material->GetPass(passType);

            VulkanGraphicPipelineDesc pipelineDesc{};
            pipelineDesc.extent = extent;
            mesh->BuildPipelineDesc(pipelineDesc);
            pass->BuildPipelineDesc(pipelineDesc);
            TPtr<VulkanGraphicPipeline> pipeline = std::make_shared<VulkanGraphicPipeline>(device, pipelineDesc, renderPass);
            usedPipelineSet.insert(pipeline);

            vkCmdBindPipeline(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetRawPipeline());


            // Vertex
            VkBuffer vertexBuffers[] = {mesh->GetVertexBuffer()->GetRawBuffer()};
            VkDeviceSize offsets[] = {0};
            vkCmdBindVertexBuffers(vkCommandBuffer, 0, 1, vertexBuffers, offsets);
            vkCmdBindIndexBuffer(vkCommandBuffer, mesh->GetIndexBuffer()->GetRawBuffer(), 0, VK_INDEX_TYPE_UINT32);

            // Fragment
            vkCmdBindDescriptorSets(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pass->GetPipelineLayout()->GetRawPipelineLayout(), 0, 1, &pass->GetDescriptorSet()->GetRawDescriptorSet(), 0, nullptr);
            vkCmdDrawIndexed(vkCommandBuffer, mesh->GetVerticesCount(), 1, 0, 0, 0);
        }
    }

    commandBuffer->EndRenderPass();

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