#include "ForwardRenderer.h"
#include "Graphic/VulkanDevice.h"
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
#include "Frame.h"
#include "RenderSystem.h"
#include "RenderTargets.h"
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
    _inFlightFence = RenderSystem::Get().GetDevice()->CreateFence(true);

    _depthPass = std::make_shared<DepthPass>();
    _directionalLightPass = std::make_shared<DirectionalLightPass>();
}

ForwardRenderer::~ForwardRenderer()
{
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
    VkFence fence = RenderSystem::Get().GetDevice()->CreateFence(false);
    transferQueue->Submit(commandBuffer, {}, {}, {}, fence);
    vkWaitForFences(RenderSystem::Get().GetDevice()->GetRawDevice(), 1, &fence, VK_TRUE, UINT64_MAX);
    RenderSystem::Get().GetDevice()->DestroyFence(fence);
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

    return objectsToRender;
}

void ForwardRenderer::RenderFrame(TPtr<VulkanCommandBuffer> commandBuffer, TPtr<Scene> scene, TPtr<Frame> frame)
{
    TPtr<VulkanDevice> device = commandBuffer->GetDevice();

    commandBuffer->Begin();

    TPtrArr<SceneObject> objectsToRender = Prepare(commandBuffer, scene);

    //Depth Pass
    TPtr<VulkanImage> depthImage = std::make_shared<VulkanImage>(device, frame->GetExtent(), VkFormat::VK_FORMAT_D32_SFLOAT, VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
    TPtr<VulkanImageView> depthImageView = std::make_shared<VulkanImageView>(depthImage, VkFormat::VK_FORMAT_D32_SFLOAT, VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT);

    TPtr<RenderTargets> depthRenderTargets = std::make_shared<RenderTargets>();
    depthRenderTargets->depthStencil = RenderTargetBinding{depthImageView, ERenderTargetLoadAction::None};
    _depthPass->Execute(objectsToRender, commandBuffer, frame, depthRenderTargets);

    //Light Pass
    TPtr<RenderTargets> lightingRenderTargets = std::make_shared<RenderTargets>();
    lightingRenderTargets->colors = {RenderTargetBinding{frame->GetFrameBuffer(), ERenderTargetLoadAction::Clear}};
    lightingRenderTargets->depthStencil = RenderTargetBinding{depthImageView, ERenderTargetLoadAction::Load};
    _directionalLightPass->Execute(objectsToRender, commandBuffer, frame, lightingRenderTargets);

    commandBuffer->End();
}


} // namespace ZE