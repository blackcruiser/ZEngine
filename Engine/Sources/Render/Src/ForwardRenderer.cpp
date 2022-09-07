#include "ForwardRenderer.h"
#include "Graphic/Surface.h"
#include "Graphic/VulkanBuffer.h"
#include "Graphic/VulkanImage.h"
#include "Graphic/VulkanImageView.h"
#include "Graphic/VulkanShader.h"
#include "Graphic/VulkanCommandBuffer.h"
#include "Graphic/VulkanCommandPool.h"
#include "Graphic/VulkanDescriptorPool.h"
#include "Graphic/VulkanDescriptorSet.h"
#include "Graphic/VulkanPipelineLayout.h"
#include "Graphic/VulkanPipeline.h"
#include "Graphic/VulkanSwapchain.h"
#include "Graphic/VulkanRenderPass.h"
#include "Graphic/VulkanFramebuffer.h"
#include "Material.h"
#include "Mesh.h"
#include "Resource/MaterialResource.h"
#include "Resource/MeshResource.h"
#include "Scene/Scene.h"
#include "Scene/SceneObject.h"
#include "Scene/TransformComponent.h"
#include "Scene/CameraComponent.h"
#include "Scene/MeshComponent.h"


namespace TE {

ForwardRenderer::ForwardRenderer(TPtr<VulkanDevice> device, TPtr<Surface> surface)
    : _device(device), _surface(surface)
{
    VkSurfaceFormatKHR surfaceFormat = _surface->GetSurfaceFormat();
    VkSurfaceCapabilitiesKHR vkCapabilities = _surface->GetCpabilities();
    _renderPass = std::make_shared<VulkanRenderPass>(_device, surfaceFormat.format);
    _swapchain = std::make_shared<VulkanSwapchain>(_device, _surface, 2);

    TPtrArr<VulkanImage> images = _swapchain->GetImages();
    TPtrArr<VulkanImageView> imageViews;
    std::transform(images.begin(), images.end(), std::back_inserter(imageViews), [&](TPtr<VulkanImage> image) {
        return std::make_shared<VulkanImageView>(_device, image);
    });

    for (int i = 0; i < 2; i++)
    {
        TPtrArr<VulkanImageView> framebufferImageViews{imageViews[i]};
        _framebuffers.push_back(std::make_shared<VulkanFramebuffer>(_device, _renderPass, framebufferImageViews, _surface->GetExtent()));
    }


    _commandPool = std::make_shared<VulkanCommandPool>(_device);
    _commandBuffer = std::make_shared<VulkanCommandBuffer>(_commandPool);

    _imageAvailableSemaphore = _device->CreateGraphicSemaphore();
    _renderFinishedSemaphore = _device->CreateGraphicSemaphore();

    _inFlightFence = _device->CreateFence(true);


    VkDescriptorPoolSize uniformPoolSize{};
    uniformPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uniformPoolSize.descriptorCount = 1;

    VkDescriptorPoolSize samplerPoolSize{};
    samplerPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerPoolSize.descriptorCount = 1;

    std::vector<VkDescriptorPoolSize> descriptorPoolSizeArr = {uniformPoolSize, samplerPoolSize};

    _descriptorPool = std::make_shared<VulkanDescriptorPool>(_device, descriptorPoolSizeArr);
}

ForwardRenderer::~ForwardRenderer()
{
    _device->DestroyGraphicSemaphore(_imageAvailableSemaphore);
    _device->DestroyGraphicSemaphore(_renderFinishedSemaphore);
    _device->DestroyFence(_inFlightFence);
}

void ForwardRenderer::Init(TPtr<Scene> scene)
{
    const TPtrArr<SceneObject>& objects = scene->GetObjects();
    for (TPtr<SceneObject> object : objects)
    {
        TPtr<MeshComponent> meshComponent = object->GetComponent<MeshComponent>();
        if (meshComponent == nullptr)
            continue;

        TPtr<MeshResource> meshResource = meshComponent->GetMesh();
        TPtr<MaterialResource> materialResource = meshComponent->GetMaterial(0);

        if (meshResource != nullptr)
        {
            TPtr<Mesh> mesh = std::make_shared<Mesh>(_device, meshResource);
            mesh->CreateVertexBuffer(_commandPool);
            mesh->CreateIndexBuffer(_commandPool);
            meshResource->SetMesh(mesh);
        }

        if (materialResource != nullptr)
        {
            TPtr<Material> material = std::make_shared<Material>(_device, materialResource);
            material->CreateGraphicShaders();
            material->CreateGraphicTextures(_commandPool);
            material->CreateDescriptorSet(_descriptorPool);
            material->UpdateDescriptorSet();
            material->CreatePipeline(meshResource->GetMesh(), _renderPass, _surface->GetExtent());
            materialResource->SetMaterial(material);
        }
    }
}

void ForwardRenderer::RenderFrame(TPtr<Scene> scene)
{
    VkDevice vkDevice = _device->GetRawDevice();
    vkWaitForFences(vkDevice, 1, &_inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(vkDevice, 1, &_inFlightFence);

    uint32_t imageIndex = _swapchain->AcquireNextImage(_imageAvailableSemaphore);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = _renderPass->GetRawRenderPass();
    renderPassInfo.framebuffer = _framebuffers[imageIndex]->GetRawFramebuffer();
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = _surface->GetExtent();

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    VkCommandBuffer vkCommandBuffer = _commandBuffer->GetRawCommandBuffer();
    _commandBuffer->Begin();

    vkCmdBeginRenderPass(vkCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    TPtr<CameraComponent> cameraComponent = scene->GetCamera();
    glm::mat4x4 VP = cameraComponent->GetProjectMatrix() * cameraComponent->GetViewMatrix();

    const TPtrArr<SceneObject>& objects = scene->GetObjects();
    for (TPtr<SceneObject> object : objects)
    {
        TPtr<MeshComponent> meshComponent = object->GetComponent<MeshComponent>();
        if (meshComponent == nullptr)
            continue;

        TPtr<MeshResource> meshResource = meshComponent->GetMesh();
        TPtr<MaterialResource> materialResource = meshComponent->GetMaterial(0);
        if (meshResource == nullptr || materialResource == nullptr)
            continue;

        TPtr<Mesh> mesh = meshResource->GetMesh();
        TPtr<Material> material = materialResource->GetMaterial();


        vkCmdBindPipeline(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material->GetPipeline()->GetRawPipeline());

        TPtr<TransformComponent> transformComponent = object->GetComponent<TransformComponent>();
        glm::mat4x4 MVP = VP * transformComponent->GetTransform();

        VkBuffer vertexBuffers[] = {mesh->GetVertexBuffer()->GetRawBuffer()};
        VkDeviceSize offsets[] = {0};
        // Vertex
        vkCmdBindVertexBuffers(vkCommandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(vkCommandBuffer, mesh->GetIndexBuffer()->GetRawBuffer(), 0, VK_INDEX_TYPE_UINT32);

        // Fragment
        material->UpdateUniformBuffer(_commandPool, MVP);

        vkCmdBindDescriptorSets(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material->GetPipelineLayout()->GetRawPipelineLayout(), 0, 1,
                                &material->GetDescriptorSet()->GetRawDescriptorSet(), 0, nullptr);
        vkCmdDrawIndexed(vkCommandBuffer, mesh->GetVerticesCount(), 1, 0, 0, 0);
    }

    vkCmdEndRenderPass(vkCommandBuffer);

    _commandBuffer->End();

    VkSemaphore waitSemaphores[] = {_imageAvailableSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    VkSemaphore signalSemaphores[] = {_renderFinishedSemaphore};
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &vkCommandBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(_device->GetGraphicQueue(), 1, &submitInfo, _inFlightFence) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkSwapchainKHR swapchains[] = {_swapchain->GetRawSwapchain()};
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional

    vkQueuePresentKHR(_device->GetPresentQueue(), &presentInfo);
}

}