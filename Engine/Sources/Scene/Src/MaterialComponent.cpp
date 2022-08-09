#include "MaterialComponent.h"


namespace TE {

MaterialComponent::MaterialComponent() : SceneComponent(EComponentType::Material)
{
}

MaterialComponent::~MaterialComponent()
{
}

void MaterialComponent::SetShader(const EMaterialShaderType& type, const std::string& shaderPath)
{
    if (_shaderMap.find(type) == _shaderMap.end())
        _shaderMap.insert(std::make_pair(type, MaterialShaderInfo()));

    MaterialShaderInfo& shaderInfo = _shaderMap[type];
    shaderInfo.filePath = shaderPath;
}

std::optional<std::reference_wrapper<MaterialShaderInfo>> MaterialComponent::GetShaderInfo(const EMaterialShaderType& type)
{
    if (_shaderMap.find(type) == _shaderMap.end())
        return std::nullopt;
    else
        return _shaderMap[type];
}

}