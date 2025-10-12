#include "Texture.h"
#include "Graphic/VulkanImage.h"
#include "RenderSystem.h"
#include "Render/RenderGraph.h"
#include "Resource/TextureResource.h"

#include <vulkan/vulkan.h>


namespace ZE {

Texture::Texture(TextureResource* textureResource)
    : _owner(textureResource)
{
}

Texture::~Texture()
{
}

void Texture::InitGraphic(RenderGraph* renderGraph)
{
    RenderResource::InitGraphic(renderGraph);

    uint32_t imageSize = _owner->GetWidth() * _owner->GetHeight() * 4;
    VkExtent3D extent{_owner->GetWidth(), _owner->GetHeight(), 1};

    _image = NewGraphicResource<VulkanImage>(renderGraph->GetDevice(), extent, VkFormat::VK_FORMAT_R8G8B8A8_SRGB);
    renderGraph->TransferImage(static_cast<const uint8_t*>(_owner->GetData()), imageSize, _image);
}

void Texture::CleanupGraphic()
{
    _image.reset();

    RenderResource::CleanupGraphic();
}

} // namespace ZE
