#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"
#include "RenderResource.h"


namespace ZE {

class TextureResource;
class RenderGraph;
class VulkanImage;
class VulkanSampler;

class Texture : public RenderResource
{
public:
    Texture(TextureResource* textureResource);
    ~Texture();

    virtual void InitGraphic(RenderGraph* renderGraph) override;
    virtual void CleanupGraphic() override;

    VulkanImage* GetImage();
    VulkanSampler* GetSampler();

private:
    TextureResource* _owner;

    VulkanImage* _image;
    VulkanSampler* _sample;
};

} // namespace ZE
