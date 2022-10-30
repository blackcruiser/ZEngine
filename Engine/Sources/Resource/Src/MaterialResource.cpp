#include "MaterialResource.h"


namespace ZE {

PassResource::PassResource()
{
}

PassResource::~PassResource()
{
}

void PassResource::Load()
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

void PassResource::Unload()
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

void PassResource::SetShader(const EShaderStage& stage, TPtr<ShaderResource> shader)
{
    if (_shaderMap.find(stage) == _shaderMap.end())
        _shaderMap.insert(std::make_pair(stage, shader));
}

TPtr<ShaderResource> PassResource::GetShader(const EShaderStage& stage)
{
    if (_shaderMap.find(stage) == _shaderMap.end())
        return nullptr;
    else
        return _shaderMap[stage];
}

const TPtrUnorderedMap<EShaderStage, ShaderResource>& PassResource::GetShaderMap()
{
    return _shaderMap;
}

void PassResource::SetTexture(const EShaderStage& stage, uint32_t bindingPoint, TPtr<TextureResource> texture)
{
    if (_textureMap.find(stage) == _textureMap.end())
        _textureMap.insert(std::make_pair(stage, std::list<TextureBindingInfo>()));

    _textureMap[stage].push_back(TextureBindingInfo{bindingPoint, texture});
}

TPtr<TextureResource> PassResource::GetTexture(const EShaderStage& stage)
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

const std::unordered_map<EShaderStage, std::list<TextureBindingInfo>>& PassResource::GetTextureMap()
{
    return _textureMap;
}


MaterialResource::MaterialResource()
    : _material(nullptr)
{
}

MaterialResource::~MaterialResource()
{
}

void MaterialResource::Load()
{
    for (auto &[passType, pass] : _passMap)
        pass->Load();
}

void MaterialResource::Unload()
{
    for (auto &[passType, pass] : _passMap)
        pass->Unload();
}

void MaterialResource::SetPass(EPassType passType, TPtr<PassResource> pass)
{
    _passMap.insert(std::make_pair(passType, pass));
}

TPtr<PassResource> MaterialResource::GetPass(EPassType passType)
{
    if (_passMap.find(passType) == _passMap.end())
        return nullptr;
    else
        return _passMap[passType];
}


void MaterialResource::SetMaterial(TPtr<Material> material)
{
    _material = material;
}

TPtr<Material> MaterialResource::GetMaterial()
{
    return _material;
}

} // namespace ZE