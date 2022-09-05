#include "MaterialResource.h"


namespace TE {

MaterialResource::MaterialResource() : _material(nullptr)
{
}

MaterialResource::~MaterialResource()
{
}

void MaterialResource::Load()
{
    for (auto& [stage, shader] : _shaderMap)
    {
        shader->Load();
    }

    for (auto& [stage, bindingInfoList] : _textureMap)
    {
        for (auto& bindingInfo : bindingInfoList)
        {
            bindingInfo.texture->Load();
        }
    }
}

void MaterialResource::Unload()
{
    for (auto& [stage, shader] : _shaderMap)
    {
        shader->Unload();
    }

    for (auto& [stage, bindingInfoList] : _textureMap)
    {
        for (auto& bindingInfo : bindingInfoList)
        {
            bindingInfo.texture->Unload();
        }
    }
}


void MaterialResource::SetShader(const EShaderStage& stage, TPtr<ShaderResource> shader)
{
    if (_shaderMap.find(stage) == _shaderMap.end())
        _shaderMap.insert(std::make_pair(stage, shader));
}

TPtr<ShaderResource> MaterialResource::GetShader(const EShaderStage& stage)
{
    if (_shaderMap.find(stage) == _shaderMap.end())
        return nullptr;
    else
        return _shaderMap[stage];
}

const TPtrUnorderedMap<EShaderStage, ShaderResource>& MaterialResource::GetShaderMap()
{
    return _shaderMap;
}

void MaterialResource::SetTexture(const EShaderStage& stage, uint32_t bindingPoint, TPtr<TextureResource> texture)
{
    if (_textureMap.find(stage) == _textureMap.end())
        _textureMap.insert(std::make_pair(stage, std::list<TextureBindingInfo>()));

    _textureMap[stage].push_back(TextureBindingInfo{bindingPoint, texture});
}

TPtr<TextureResource> MaterialResource::GetTexture(const EShaderStage& stage)
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

const std::unordered_map<EShaderStage, std::list<TextureBindingInfo>>& MaterialResource::GetTextureMap()
{
    return _textureMap;
}

void MaterialResource::SetMaterial(TPtr<Material> material)
{
    _material = material;
}

TPtr<Material> MaterialResource::GetMaterial()
{
    return _material;
}

} // namespace TE