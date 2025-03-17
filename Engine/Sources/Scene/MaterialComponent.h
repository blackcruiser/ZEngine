#pragma once

#include "CoreDefines.h"
#include "Render/Shader.h"
#include "Render/Texture.h"
#include "SceneComponent.h"

#include <filesystem>
#include <string>
#include <unordered_map>

namespace TE {

class RSMaterial;

struct TextureBindingInfo
{
    uint32_t bindingPoint;
    TPtr<Texture> texture;
};

class MaterialComponent : public SceneComponent
{
public:
    MaterialComponent();
    virtual ~MaterialComponent();

    virtual void Load() override;

    void SetShader(const EShaderStage& stage, const std::filesystem::path& shaderPath);
    TPtr<Shader> GetShader(const EShaderStage& stage);
    const TPtrUnorderedMap<EShaderStage, Shader>& GetShaderMap();

    void SetTexture(const EShaderStage& stage, uint32_t bindingPoint, TPtr<Texture> texture);
    TPtr<Texture> GetTexture(const EShaderStage& stage);
    const std::unordered_map<EShaderStage, std::list<TextureBindingInfo>>& GetTextureMap();

private:
    // EBlendType _blendType;
    TPtrUnorderedMap<EShaderStage, Shader> _shaderMap;
    std::unordered_map<EShaderStage, std::list<TextureBindingInfo>> _textureMap;

public:
    TPtr<RSMaterial> rsMaterial;
};

} // namespace TE