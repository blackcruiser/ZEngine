#include "ForwardRenderer.h"
#include "RenderSystem.h"
#include "RenderGraph.h"
#include "RenderTargets.h"
#include "Viewport.h"
#include "Graphic/VulkanImage.h"
#include "Graphic/VulkanImageView.h"
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
    _depthPass = std::make_shared<DepthPass>();
    _directionalLightPass = std::make_shared<DirectionalLightPass>();
}

ForwardRenderer::~ForwardRenderer()
{
}

void ForwardRenderer::Init(TPtr<RenderGraph> renderGraph, TPtr<Scene> scene)
{
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
            mesh->CreateVertexBuffer(renderGraph);
            mesh->CreateIndexBuffer(renderGraph);
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
                    pass->BuildRenderResource(renderGraph);
                }
            }
        }
    }

    renderGraph->Execute({}, {}, {});
}

TPtrArr<SceneObject> ForwardRenderer::Prepare(TPtr<RenderGraph> renderGraph, TPtr<Scene> scene)
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
                    pass->UpdateUniformBuffer(renderGraph, MVP);
                }
            }
        }
    }

    return objectsToRender;
}

void ForwardRenderer::RenderFrame(TPtr<RenderGraph> renderGraph, TPtr<Viewport> viewport, TPtr<Scene> scene)
{
    TPtrArr<SceneObject> objectsToRender = Prepare(renderGraph, scene);

    glm::ivec2 size = viewport->GetSize();
    VkExtent3D extent { size.r, size.g, 1.0f };
    //Depth Pass
    TPtr<VulkanImage> depthImage = std::make_shared<VulkanImage>(renderGraph->GetDevice(), extent, VkFormat::VK_FORMAT_D32_SFLOAT, VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
    TPtr<VulkanImageView> depthImageView = std::make_shared<VulkanImageView>(depthImage, VkFormat::VK_FORMAT_D32_SFLOAT, VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT);

    TPtr<RenderTargets> depthRenderTargets = std::make_shared<RenderTargets>();
    depthRenderTargets->depthStencil = RenderTargetBinding{depthImageView, ERenderTargetLoadAction::Clear};
    renderGraph->SetRenderTargets(depthRenderTargets);
    _depthPass->Execute(renderGraph, objectsToRender);

    //Light Pass
    TPtr<VulkanImage> backBuffer = viewport->GetCurrentImage();
    TPtr<VulkanImageView> backBufferView = std::make_shared<VulkanImageView>(backBuffer);
    TPtr<RenderTargets> lightingRenderTargets = std::make_shared<RenderTargets>();
    lightingRenderTargets->colors = {RenderTargetBinding{backBufferView, ERenderTargetLoadAction::Clear}};
    lightingRenderTargets->depthStencil = RenderTargetBinding{depthImageView, ERenderTargetLoadAction::Load};
    renderGraph->SetRenderTargets(lightingRenderTargets);
    _directionalLightPass->Execute(renderGraph, objectsToRender);
}

} // namespace ZE