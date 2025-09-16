#include "TransformComponent.h"
#include "Graphic/GraphicResource.h"
#include "Graphic/VulkanBuffer.h"
#include "Render/RenderGraph.h"


namespace ZE {

TransformComponent::TransformComponent() : SceneComponent(EComponentType::Transform), _transform(1)
{
}

TransformComponent::~TransformComponent()
{
}

void TransformComponent::SetTransform(const glm::mat4x4& transform)
{
    _transform = transform;
}

const glm::mat4x4& TransformComponent::GetTransform()
{
    return _transform;
}


TransformComponentResource::TransformComponentResource(TransformComponent* component)
{
}

TransformComponentResource::~TransformComponentResource()
{
}

void TransformComponentResource::InitGraphic(RenderGraph* renderGraph)
{
    _MVPBuffer = NewGraphicResource<VulkanBuffer>(renderGraph->GetDevice(), sizeof(glm::mat4x4), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    renderGraph->TransferBuffer(reinterpret_cast<const uint8_t*>(&owner->GetTransform()), sizeof(glm::mat4x4), _MVPBuffer);
}

}