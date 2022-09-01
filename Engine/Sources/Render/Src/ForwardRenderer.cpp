#include "ForwardRenderer.h"
#include "Graphic/CommandBuffer.h"
#include "Graphic/CommandPool.h"
#include "Graphic/Surface.h"
#include "Graphic/VulkanImage.h"
#include "Graphic/VulkanShader.h"
#include "RSMaterial.h"
#include "Scene/CameraComponent.h"
#include "Scene/MaterialComponent.h"
#include "Scene/MeshComponent.h"
#include "Scene/Scene.h"
#include "Scene/SceneObject.h"
#include "Scene/TransformComponent.h"
#include "Shader.h"

#include <algorithm>
#include <array>
#include <cstddef>

#include <functional>
#include <stdexcept>


namespace TE {


ForwardRenderer::ForwardRenderer(TPtr<Device> device, TPtr<Surface> surface)
    : _device(device), _surface(surface), _stagingBuffer(VK_NULL_HANDLE), _stagingBufferSize(0),
      _vertexBuffer(VK_NULL_HANDLE), _vertexBufferSize(0), _indexBuffer(VK_NULL_HANDLE), _indexesBufferSize(0)
{
    VkSurfaceFormatKHR surfaceFormat = _surface->GetSurfaceFormat();
    _vkRenderPass = _device->CreateRenderPass(surfaceFormat.format);
    CreateSwapchain(_vkRenderPass);

    _commandPool = std::make_shared<CommandPool>(_device);
    _commandBuffer = _commandPool->CreateCommandBuffer(_commandPool);

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

    _descriptorPool = _device->CreateDescriptorPool(descriptorPoolSizeArr);
}

ForwardRenderer::~ForwardRenderer()
{
    _device->FreeMemory(_indexBufferMemory);
    _device->DestroyBuffer(_indexBuffer);

    _device->FreeMemory(_vertexBufferMemory);
    _device->DestroyBuffer(_vertexBuffer);

    _device->FreeMemory(_stagingBufferMemory);
    _device->DestroyBuffer(_stagingBuffer);

    _device->DestroyDescriptorPool(_descriptorPool);

    _device->DestroyGraphicSemaphore(_imageAvailableSemaphore);
    _device->DestroyGraphicSemaphore(_renderFinishedSemaphore);
    _device->DestroyFence(_inFlightFence);

    _device->DestroyRenderPass(_vkRenderPass);


    for (auto imageView : _vkImageViews)
    {
        _device->DestroyImageView(imageView);
    }

    for (VkFramebuffer& framebuffer : _vkFramebuffers)
    {
        _device->DestroyFramebuffer(framebuffer);
    }

    _commandPool->DestroyCommandBuffer(_commandBuffer);
    _commandPool.reset();
    _device->DestroySwapchain(_vkSwapchain);
}

VkPipeline ForwardRenderer::CreatePipeline(TPtr<MeshComponent> meshComponent, TPtr<RSMaterial> material)
{
    // Shader
    TPtr<VulkanShader> vertexShader = material->GetGraphicShader(EShaderStage::Vertex);
    TPtr<VulkanShader> fragmentShader = material->GetGraphicShader(EShaderStage::Fragment);


    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    std::vector<VkVertexInputAttributeDescription> attributeDescriptions(3);
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, pos);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, normal);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(Vertex, texCoord);


    if (material->descriptorSet == VK_NULL_HANDLE)
    {
        CreateDescriptorSet(meshComponent, material);
    }

    material->vkPipelineLayout = _device->CreatePipelineLayout(material->descriptorSetLayout);
    VkPipeline vkPipeline = _device->CreateGraphicPipeline(
        vertexShader->GetRawShader(), fragmentShader->GetRawShader(), _surface->GetExtent(), bindingDescription,
        attributeDescriptions, material->vkPipelineLayout, _vkRenderPass);

    return vkPipeline;
}

void ForwardRenderer::CreateSwapchain(VkRenderPass renderPass)
{
    VkSurfaceFormatKHR vkSurfaceFormat = _surface->GetSurfaceFormat();
    VkPresentModeKHR vkPresentMode = _surface->GetPresentMode();
    VkSurfaceCapabilitiesKHR vkCapabilities = _surface->GetCpabilities();
    VkExtent2D extent = _surface->GetExtent();

    _vkSwapchain =
        _device->CreateSwapchain(vkCapabilities.minImageCount + 1, vkSurfaceFormat.format, vkSurfaceFormat.colorSpace,
                                 extent, vkCapabilities.currentTransform, vkPresentMode);

    uint32_t imageCount;
    vkGetSwapchainImagesKHR(_device->GetRawDevice(), _vkSwapchain, &imageCount, nullptr);
    _vkImages.resize(imageCount);
    vkGetSwapchainImagesKHR(_device->GetRawDevice(), _vkSwapchain, &imageCount, _vkImages.data());

    // Image View
    _vkImageViews.resize(imageCount);
    for (size_t i = 0; i < imageCount; i++)
    {
        _vkImageViews[i] = _device->CreateImageView(_vkImages[i], vkSurfaceFormat.format);
    }

    _vkFramebuffers.resize(imageCount);
    for (size_t i = 0; i < imageCount; i++)
    {
        std::vector<VkImageView> attachments = {_vkImageViews[i]};

        _vkFramebuffers[i] = _device->CreateFramebuffer(renderPass, attachments, extent.width, extent.height);
    }
}

void ForwardRenderer::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    CommandBuffer* commandBuffer = _commandPool->CreateCommandBuffer(_commandPool);
    VkCommandBuffer vkCommandBuffer = commandBuffer->GetRawCommandBuffer();

    commandBuffer->Begin();

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(vkCommandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    commandBuffer->End();

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &vkCommandBuffer;

    vkQueueSubmit(_device->GetGraphicQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(_device->GetGraphicQueue());

    _commandPool->DestroyCommandBuffer(commandBuffer);
}


void ForwardRenderer::RenderFrame(TPtr<Scene> scene)
{
    VkDevice vkDevice = _device->GetRawDevice();
    vkWaitForFences(vkDevice, 1, &_inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(vkDevice, 1, &_inFlightFence);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(vkDevice, _vkSwapchain, UINT64_MAX, _imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = _vkRenderPass;
    renderPassInfo.framebuffer = _vkFramebuffers[imageIndex];
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
        TPtr<MaterialComponent> materialComponent = object->GetComponent<MaterialComponent>();
        TPtr<MeshComponent> meshComponent = object->GetComponent<MeshComponent>();
        if (materialComponent == nullptr || meshComponent == nullptr)
            continue;

        if (materialComponent->rsMaterial == nullptr)
        {
            materialComponent->rsMaterial = std::make_shared<RSMaterial>(_device, materialComponent);
            materialComponent->rsMaterial->CreateGraphicTextures(_device, _commandPool);
            materialComponent->rsMaterial->CreateGraphicShaders(_device);

            materialComponent->rsMaterial->vkPipeline = CreatePipeline(meshComponent, materialComponent->rsMaterial);
        }
        TPtr<RSMaterial> rsMaterial = materialComponent->rsMaterial;

        vkCmdBindPipeline(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, rsMaterial->vkPipeline);

        const std::vector<Vertex>& vertices = meshComponent->GetVertices();
        const std::vector<uint32_t>& indexes = meshComponent->GetIndexes();

        TPtr<TransformComponent> transformComponent = object->GetComponent<TransformComponent>();
        glm::mat4x4 MVP = VP * transformComponent->GetTransform();

        // Vertex
        size_t curVertexBufferSize = vertices.size() * sizeof(Vertex);
        size_t curIndexBufferSize = indexes.size() * sizeof(int);

        if (_stagingBuffer != VK_NULL_HANDLE &&
            (curVertexBufferSize > _stagingBufferSize || curIndexBufferSize > _stagingBufferSize))
        {
            _device->FreeMemory(_stagingBufferMemory);
            _device->DestroyBuffer(_stagingBuffer);

            _stagingBuffer = VK_NULL_HANDLE;
        }

        if (_stagingBuffer == VK_NULL_HANDLE)
        {
            _stagingBufferSize = std::max(curIndexBufferSize, curVertexBufferSize);
            _stagingBuffer = _device->CreateBuffer(_stagingBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
            _stagingBufferMemory = _device->AllocateAndBindBufferMemory(
                _stagingBuffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        }

        if (_vertexBuffer != VK_NULL_HANDLE && curVertexBufferSize > _vertexBufferSize)
        {
            _device->FreeMemory(_vertexBufferMemory);
            _device->DestroyBuffer(_vertexBuffer);

            _vertexBuffer = VK_NULL_HANDLE;
        }

        if (_vertexBuffer == VK_NULL_HANDLE)
        {
            _vertexBufferSize = curVertexBufferSize;
            _vertexBuffer = _device->CreateBuffer(curVertexBufferSize,
                                                  VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
            _vertexBufferMemory =
                _device->AllocateAndBindBufferMemory(_vertexBuffer, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        }

        if (_indexBuffer != VK_NULL_HANDLE && curIndexBufferSize > _indexesBufferSize)
        {
            _device->FreeMemory(_indexBufferMemory);
            _device->DestroyBuffer(_indexBuffer);

            _indexBuffer = VK_NULL_HANDLE;
        }

        if (_indexBuffer == VK_NULL_HANDLE)
        {
            _indexesBufferSize = curIndexBufferSize;
            _indexBuffer = _device->CreateBuffer(curIndexBufferSize,
                                                 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
            _indexBufferMemory =
                _device->AllocateAndBindBufferMemory(_indexBuffer, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        }

        void* verticesDataPtr = nullptr;
        vkMapMemory(_device->GetRawDevice(), _stagingBufferMemory, 0, curVertexBufferSize, 0, &verticesDataPtr);
        memcpy(verticesDataPtr, vertices.data(), curVertexBufferSize);
        vkUnmapMemory(_device->GetRawDevice(), _stagingBufferMemory);

        CopyBuffer(_stagingBuffer, _vertexBuffer, curVertexBufferSize);


        void* indexesDataPtr = nullptr;
        vkMapMemory(_device->GetRawDevice(), _stagingBufferMemory, 0, curIndexBufferSize, 0, &indexesDataPtr);
        memcpy(indexesDataPtr, indexes.data(), curIndexBufferSize);
        vkUnmapMemory(_device->GetRawDevice(), _stagingBufferMemory);

        CopyBuffer(_stagingBuffer, _indexBuffer, curIndexBufferSize);

        VkBuffer vertexBuffers[] = {_vertexBuffer};
        VkDeviceSize offsets[] = {0};

        vkCmdBindVertexBuffers(vkCommandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(vkCommandBuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT32);


        // Fragment
        if (rsMaterial->_uniformBuffer == VK_NULL_HANDLE)
        {
            rsMaterial->_uniformBuffer = _device->CreateBuffer(sizeof(glm::mat4x4), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
            rsMaterial->_uniformBufferMemory = _device->AllocateAndBindBufferMemory(
                rsMaterial->_uniformBuffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        }

        void* uniformDataPtr = nullptr;
        vkMapMemory(_device->GetRawDevice(), rsMaterial->_uniformBufferMemory, 0, sizeof(glm::mat4x4), 0,
                    &uniformDataPtr);
        memcpy(uniformDataPtr, &MVP, sizeof(glm::mat4x4));
        vkUnmapMemory(_device->GetRawDevice(), rsMaterial->_uniformBufferMemory);

        UpdateDescriptorSet(rsMaterial->descriptorSet, meshComponent, rsMaterial);

        vkCmdBindDescriptorSets(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, rsMaterial->vkPipelineLayout, 0, 1,
                                &rsMaterial->descriptorSet, 0, nullptr);
        vkCmdDrawIndexed(vkCommandBuffer, indexes.size(), 1, 0, 0, 0);
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

    VkSwapchainKHR swapchains[] = {_vkSwapchain};
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


void ForwardRenderer::CreateDescriptorSet(TPtr<MeshComponent> meshComponent, TPtr<RSMaterial> material)
{
    VkDescriptorSetLayoutBinding uniformDescriptorSetLayoutBinding{};
    uniformDescriptorSetLayoutBinding.binding = 0;
    uniformDescriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uniformDescriptorSetLayoutBinding.descriptorCount = 1;
    uniformDescriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutBinding samplerDescriptorSetlayoutBinding{};
    samplerDescriptorSetlayoutBinding.binding = 1;
    samplerDescriptorSetlayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerDescriptorSetlayoutBinding.descriptorCount = 1;
    samplerDescriptorSetlayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings = {uniformDescriptorSetLayoutBinding,
                                                                             samplerDescriptorSetlayoutBinding};

    material->descriptorSetLayout = _device->CreateDescriptorSetLayout(descriptorSetLayoutBindings);

    material->descriptorSet = _device->AllocateDescriptorSet(_descriptorPool, 1, &material->descriptorSetLayout);
}


void ForwardRenderer::UpdateDescriptorSet(VkDescriptorSet descriptorSet, TPtr<MeshComponent> meshComponent,
                                          TPtr<RSMaterial> RSMaterial)
{
    std::optional<std::reference_wrapper<std::list<VulkanImageBindingInfo>>> textureBindingInfoWrap =
        RSMaterial->GetGraphicTexture(EShaderStage::Fragment);
    std::list<VulkanImageBindingInfo> textureBindingInfo = textureBindingInfoWrap.value();

    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = RSMaterial->_uniformBuffer;
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(glm::mat4x4);

    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = textureBindingInfo.begin()->vulkanImage->GetRawImageView();
    imageInfo.sampler = textureBindingInfo.begin()->vulkanImage->GetRawSampler();

    VkWriteDescriptorSet uniformBufferdescriptorWrite{};
    uniformBufferdescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    uniformBufferdescriptorWrite.dstSet = descriptorSet;
    uniformBufferdescriptorWrite.dstBinding = 0;
    uniformBufferdescriptorWrite.dstArrayElement = 0;
    uniformBufferdescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uniformBufferdescriptorWrite.descriptorCount = 1;
    uniformBufferdescriptorWrite.pBufferInfo = &bufferInfo;

    VkWriteDescriptorSet samplerBufferdescriptorWrite{};
    samplerBufferdescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    samplerBufferdescriptorWrite.dstSet = descriptorSet;
    samplerBufferdescriptorWrite.dstBinding = 1;
    samplerBufferdescriptorWrite.dstArrayElement = 0;
    samplerBufferdescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerBufferdescriptorWrite.descriptorCount = 1;
    samplerBufferdescriptorWrite.pImageInfo = &imageInfo;

    std::vector<VkWriteDescriptorSet> descriptorWrites{uniformBufferdescriptorWrite, samplerBufferdescriptorWrite};

    _device->UpdateDescriptorSet(descriptorWrites);
}

}