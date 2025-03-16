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

VkAttachmentLoadOp ConvertRenderTargetLoadActionToVulkan(ERenderTargetLoadAction loadAction)
{
    switch (loadAction)
    {
    case ERenderTargetLoadAction::Load:
        return VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_LOAD;
    case ERenderTargetLoadAction::Clear:
        return VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR;
    case ERenderTargetLoadAction::DontCare:
        return VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    }

    return VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE;
}

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

void ForwardRenderer::SetupFrame(TPtr<VulkanCommandBuffer> commandBuffer, TPtr<Frame> frame)
{
    TPtr<VulkanDevice> device = commandBuffer->GetDevice();

    //Depth Pass
    TPtr<VulkanImage> depthImage = std::make_shared<VulkanImage>(device, frame->GetExtent(), VkFormat::VK_FORMAT_D32_SFLOAT, VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
    TPtr<VulkanImageView> depthImageView = std::make_shared<VulkanImageView>(depthImage, VkFormat::VK_FORMAT_D32_SFLOAT, VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT);
    TPtr<RenderTargets> depthRenderTargets = std::make_shared<RenderTargets>();
    depthRenderTargets->depthStencil = RenderTargetBinding{depthImageView, ERenderTargetLoadAction::Clear};

    _depthPass->Setup(depthImageView);

    TPtr<RenderTargets> lightingRenderTargets = std::make_shared<RenderTargets>();
    lightingRenderTargets->colors = {RenderTargetBinding{frame->GetFrameBuffer(), ERenderTargetLoadAction::Clear}};
    lightingRenderTargets->depthStencil = RenderTargetBinding{depthImageView, ERenderTargetLoadAction::Load};

    _directionalLightPass->Setup(frame->GetFrameBuffer(), depthImageView);
}

void ForwardRenderer::RenderFrame(TPtr<VulkanCommandBuffer> commandBuffer, TPtr<Scene> scene, TPtr<Frame> frame)
{
    TPtr<VulkanDevice> device = commandBuffer->GetDevice();

    commandBuffer->Begin();

    TPtrArr<SceneObject> objectsToRender = Prepare(commandBuffer, scene);

    for (TPtr<RenderPass>& renderPass : _passes)
    {
        RenderTargets renderTargets = renderPass->GetRenderTargets();

        VkExtent3D extent3D = renderTargets.colors.empty() ? renderTargets.depthStencil.value().target->GetExtent() : renderTargets.colors[0].target->GetExtent();
        VkExtent2D extent2D{extent3D.width, extent3D.height};

        TPtrArr<VulkanImageView> framebufferImageArr;
        std::vector<VkAttachmentDescription> colorAttachmentArr;
        std::vector<VkClearValue> clearValues;

        TPtr<VulkanRenderPass> vkRenderPass;
        for (auto& bindings : renderTargets.colors)
        {
            TPtr<VulkanImageView> imageView = bindings.target;

            VkAttachmentDescription attachment{};
            attachment.format = imageView->GetImage()->GetFormat();
            attachment.samples = VK_SAMPLE_COUNT_1_BIT;
            attachment.loadOp = ConvertRenderTargetLoadActionToVulkan(bindings.loadAction);
            attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attachment.initialLayout = imageView->GetImage()->GetLayout();
            attachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            colorAttachmentArr.emplace_back(attachment);
            framebufferImageArr.emplace_back(imageView);

            VkClearValue clearValue;
            clearValue.color = {0.0f, 0.0f, 0.0f, 0.0f};
            clearValues.push_back(clearValue);

            imageView->GetImage()->SetLayout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
        }

        if (renderTargets.depthStencil.has_value())
        {
            RenderTargetBinding& depthBinding = renderTargets.depthStencil.value();
            VkAttachmentDescription depthAttachment{};
            depthAttachment.format = depthBinding.target->GetFormat();
            depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
            depthAttachment.loadOp = ConvertRenderTargetLoadActionToVulkan(depthBinding.loadAction);
            depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            depthAttachment.initialLayout = depthBinding.target->GetImage()->GetLayout();
            depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            vkRenderPass = std::make_shared<VulkanRenderPass>(device, colorAttachmentArr, depthAttachment);
            framebufferImageArr.emplace_back(renderTargets.depthStencil.value().target);

            VkClearValue clearValue;
            clearValue.depthStencil.depth = 0.0f;
            clearValue.depthStencil.stencil = 0;
            clearValues.push_back(clearValue);

            depthBinding.target->GetImage()->SetLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
        }
        else
        {
            vkRenderPass = std::make_shared<VulkanRenderPass>(device, colorAttachmentArr);
        }

        TPtr<VulkanFramebuffer> framebuffer = std::make_shared<VulkanFramebuffer>(device, vkRenderPass, framebufferImageArr, extent2D);
        frame->PutFramebuffer(framebuffer);
        commandBuffer->BeginRenderPass(vkRenderPass, framebuffer, {{0, 0}, extent2D}, clearValues);

        renderPass->Execute(objectsToRender, commandBuffer, frame->GetViewport());

        commandBuffer->EndRenderPass();
    }

    frame->GetFrameBuffer()->GetImage()->TransitionLayout(commandBuffer, VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VkImageLayout::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    commandBuffer->End();
}


} // namespace ZE