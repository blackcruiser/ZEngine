#include "MaterialComponent.h"


namespace TE {

MaterialComponent::MaterialComponent() : SceneComponent(EComponentType::Material)
{
}

MaterialComponent::~MaterialComponent()
{
}

void MaterialComponent::Load()
{
    // for (auto& [stage, shader] : _shaderMap)
    // {
    //     shader->Load();
    // }

    for (auto& [stage, bindingInfoList] : _textureMap)
    {
        for (auto& bindingInfo : bindingInfoList)
        {
            bindingInfo.texture->Load();
        }
    }
}

void MaterialComponent::SetShader(const EShaderStage& stage, const std::filesystem::path& shaderPath)
{
    TPtr<Shader> shader = std::make_shared<Shader>(shaderPath);
    if (_shaderMap.find(stage) == _shaderMap.end())
        _shaderMap.insert(std::make_pair(stage, shader));
}

TPtr<Shader> MaterialComponent::GetShader(const EShaderStage& stage)
{
    if (_shaderMap.find(stage) == _shaderMap.end())
        return nullptr;
    else
        return _shaderMap[stage];
}

const TPtrUnorderedMap<EShaderStage, Shader>& MaterialComponent::GetShaderMap()
{
    return _shaderMap;
}

void MaterialComponent::SetTexture(const EShaderStage& stage, uint32_t bindingPoint, TPtr<Texture> texture)
{
    if (_textureMap.find(stage) == _textureMap.end())
        _textureMap.insert(std::make_pair(stage, std::list<TextureBindingInfo>()));

    _textureMap[stage].push_back(TextureBindingInfo{bindingPoint, texture});
}

TPtr<Texture> MaterialComponent::GetTexture(const EShaderStage& stage)
{
    if (_textureMap.find(stage) == _textureMap.end())
        return nullptr;
    else
    {
        std::list<TextureBindingInfo>& textureBindingInfoList = _textureMap[stage];
        if (textureBindingInfoList.empty())
            return nullptr;
        else
        {
            const TextureBindingInfo& bindingInfo = textureBindingInfoList.front();
            return bindingInfo.texture;
        }
    }
}

const std::unordered_map<EShaderStage, std::list<TextureBindingInfo>>& MaterialComponent::GetTextureMap()
{
    return _textureMap;
}

} // namespace TE