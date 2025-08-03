#include "ForwardRenderer.h"
#include "RenderSystem.h"
#include "RenderGraph.h"
#include "RenderTargets.h"
#include "Viewport.h"
#include "Graphic/VulkanImage.h"
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

void ForwardRenderer::Init(TPtr<RenderGraph> renderGraph, Viewport* viewport)
{
    glm::ivec2 size = viewport->GetSize();
    VkExtent3D extent{size.r, size.g, 1.0f};

    // Depth Pass
    _depthRenderTarget = TPtr<VulkanImage>(new VulkanImage(renderGraph->GetDevice(), extent, VkFormat::VK_FORMAT_D32_SFLOAT, VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT), GraphicResourceDeleter());
}

void ForwardRenderer::Cleanup(TPtr<RenderGraph> renderGraph)
{
    _depthRenderTarget.reset();
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

void ForwardRenderer::RenderFrame(TPtr<RenderGraph> renderGraph, Viewport* viewport, TPtr<Scene> scene)
{
    {
        TPtr<VulkanImage> backBuffer = viewport->GetCurrentImage();
        _depthPass->Init(_depthRenderTarget);
        _directionalLightPass->Init(backBuffer, _depthRenderTarget);
    }

    {
        TPtrArr<SceneObject> objectsToRender = Prepare(renderGraph, scene);
        _depthPass->Execute(renderGraph, objectsToRender);
        _directionalLightPass->Execute(renderGraph, objectsToRender);
    }
}

} // namespace ZE