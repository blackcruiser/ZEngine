#pragma once

#include "CoreDefines.h"
#include "CoreTypes.h"
#include "ZEMaterialTypes.h"
#include "BaseResource.h"
#include "ShaderResource.h"
#include "TextureResource.h"

#include <string>
#include <list>
#include <filesystem>
#include <unordered_map>

namespace ZE {

class Material;

enum class EPassType : int
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

    void SetCullintType(ECullingType cullingType);
    ECullingType GetCullingType();
    
    void SetBlendState(uint32_t index, const BlendState& blendState);
    const BlendState& GetBlendState(uint32_t index);
    const std::vector<BlendState>& GetBlendStates();

    void SetDepthStencilState(const DepthStencilState& depthStencilState);
    const DepthStencilState& GetDepthStencilState();

    void SetShader(const EShaderStage& stage, TPtr<ShaderResource> shader);
    TPtr<ShaderResource> GetShader(const EShaderStage& stage);
    const TPtrUnorderedMap<EShaderStage, ShaderResource>& GetShaderMap();

    void SetTexture(const EShaderStage& stage, uint32_t bindingPoint, TPtr<TextureResource> texture);
    TPtr<TextureResource> GetTexture(const EShaderStage& stage);
    const std::unordered_map<EShaderStage, std::list<TextureBindingInfo>>& GetTextureMap();

private:
    ECullingType _cullingType;
    DepthStencilState _depthStencilState;
    std::vector<BlendState> _blendStates;
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

    void SetPass(EPassType passType, TPtr<PassResource> pass);
    TPtr<PassResource> GetPass(EPassType passType);

private:
    TPtrUnorderedMap<EPassType, PassResource> _passMap;

public:
    // ToDo
    void SetMaterial(TPtr<Material> material);
    TPtr<Material> GetMaterial();

private:
    TPtr<Material> _material;
};

} // namespace ZE
