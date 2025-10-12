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

void ForwardRenderer::Init(RenderGraph* renderGraph, Viewport* viewport)
{
    glm::ivec2 size = viewport->GetSize();
    VkExtent3D extent{size.r, size.g, 1.0f};

    // Depth Pass
    _depthRenderTarget = NewGraphicResource<VulkanImage>(renderGraph->GetDevice(), extent, VkFormat::VK_FORMAT_D32_SFLOAT, VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

void ForwardRenderer::Cleanup(RenderGraph* renderGraph)
{
    _depthRenderTarget.reset();
}

void ForwardRenderer::RenderFrame(RenderGraph* renderGraph, Viewport* viewport, SceneResource* sceneResource)
{
    {
        TPtr<VulkanImage> backBuffer = viewport->GetCurrentImage();
        _depthPass->Init(_depthRenderTarget);
        _directionalLightPass->Init(backBuffer, _depthRenderTarget);
    }

    {
        _depthPass->Execute(renderGraph, sceneResource);
        renderGraph->Execute();

        _directionalLightPass->Execute(renderGraph, sceneResource);
        renderGraph->Execute();
    }
}

} // namespace ZE