#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"
#include "BaseResource.h"
#include "ShaderResource.h"
#include "TextureResource.h"

#include <string>
#include <list>
#include <filesystem>
#include <unordered_map>

namespace ZE {

class Material;

struct TextureBindingInfo
{
    uint32_t bindingPoint;
    TPtr<TextureResource> texture;
};

class MaterialResource : public BaseResource
{
public:
    MaterialResource();
    virtual ~MaterialResource();

    virtual void Load() override;
    virtual void Unload() override;

    void SetShader(const EShaderStage& stage, TPtr<ShaderResource> shader);
    TPtr<ShaderResource> GetShader(const EShaderStage& stage);
    const TPtrUnorderedMap<EShaderStage, ShaderResource>& GetShaderMap();

    void SetTexture(const EShaderStage& stage, uint32_t bindingPoint, TPtr<TextureResource> texture);
    TPtr<TextureResource> GetTexture(const EShaderStage& stage);
    const std::unordered_map<EShaderStage, std::list<TextureBindingInfo>>& GetTextureMap();

    void SetMaterial(TPtr<Material> material);
    TPtr<Material> GetMaterial();

private:
    // EBlendType _blendType;
    TPtrUnorderedMap<EShaderStage, ShaderResource> _shaderMap;
    std::unordered_map<EShaderStage, std::list<TextureBindingInfo>> _textureMap;

    TPtr<Material> _material;
};

} // namespace ZE
