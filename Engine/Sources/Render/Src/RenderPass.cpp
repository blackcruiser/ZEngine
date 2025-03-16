#include "RenderPass.h"
#include "RenderTargets.h"
#include "Frame.h"
#include "Graphic/VulkanCommandBuffer.h"
#include "Graphic/VulkanRenderPass.h"
#include "Graphic/VulkanFramebuffer.h"
#include "Graphic/VulkanImage.h"
#include "Graphic/VulkanImageView.h"
#include "Graphic/VulkanBuffer.h"

namespace ZE {



RenderPass::RenderPass()
{
}

RenderPass::~RenderPass()
{
}

void RenderPass::Execute(TPtrArr<SceneObject> objectsToRender, TPtr<VulkanCommandBuffer> commandBuffer, const glm::ivec2& viewportSize)
{
    VkViewport viewport{0.0f, 0.0f, static_cast<float>(viewportSize.x), static_cast<float>(viewportSize.y), 0.0f, 1.0f};
    vkCmdSetViewport(commandBuffer->GetRawCommandBuffer(), 0, 1, &viewport);

    VkRect2D scissor{0, 0, static_cast<int32_t>(viewportSize.x), static_cast<int32_t>(viewportSize.y)};
    vkCmdSetScissor(commandBuffer->GetRawCommandBuffer(), 0, 1, &scissor);

    Draw(objectsToRender, commandBuffer);

    commandBuffer->EndRenderPass();
}

} // namespace ZE