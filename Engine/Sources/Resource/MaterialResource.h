#pragma once

#include "ZEDefine.h"
#include "ZEType.h"
#include "BaseResource.h"
#include "ShaderResource.h"
#include "TextureResource.h"

#include <string>
#include <list>
#include <filesystem>
#include <unordered_map>

namespace ZE {

class Material;

enum class PassType : int
{
    BasePass = 0,
    DepthPass = 1,
    PassCount
};


struct TextureBindingInfo
{
    uint32_t bindingPoint;
    TPtr<TextureResource> texture;
};

class PassResource : public BaseResource
{
public:
    PassResource();
    virtual ~PassResource();

    virtual void Load() override;
    virtual void Unload() override;

    void SetShader(const EShaderStage& stage, TPtr<ShaderResource> shader);
    TPtr<ShaderResource> GetShader(const EShaderStage& stage);
    const TPtrUnorderedMap<EShaderStage, ShaderResource>& GetShaderMap();

    void SetTexture(const EShaderStage& stage, uint32_t bindingPoint, TPtr<TextureResource> texture);
    TPtr<TextureResource> GetTexture(const EShaderStage& stage);
    const std::unordered_map<EShaderStage, std::list<TextureBindingInfo>>& GetTextureMap();

private:
    // EBlendType _blendType;
    TPtrUnorderedMap<EShaderStage, ShaderResource> _shaderMap;
    std::unordered_map<EShaderStage, std::list<TextureBindingInfo>> _textureMap;
};

class MaterialResource : public BaseResource
{
public:
    MaterialResource();
    virtual ~MaterialResource();

    virtual void Load() override;
    virtual void Unload() override;

    void SetPass(PassType passType, TPtr<PassResource> pass);
    TPtr<PassResource> GetPass(PassType passType);

private:
    TPtrUnorderedMap<PassType, PassResource> _passMap;

public:
    // ToDo
    void SetMaterial(TPtr<Material> material);
    TPtr<Material> GetMaterial();

private:
    TPtr<Material> _material;
};

} // namespace ZE
