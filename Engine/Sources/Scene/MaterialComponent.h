#pragma once

#include "CoreDefines.h"
#include "SceneComponent.h"

#include <string>
#include <optional>
#include <filesystem>


namespace TE {

enum class EMaterialShaderType : int
{
    Vertex,
    Fragment,
};


struct MaterialShaderInfo
{
    std::filesystem::path filePath;
    std::filesystem::path bytecodePath;
};


class MaterialComponent : public SceneComponent
{
public:
    MaterialComponent();
    ~MaterialComponent();

    void SetShader(const EMaterialShaderType& type, const std::string& shaderPath);
    std::optional<std::reference_wrapper<MaterialShaderInfo>> GetShaderInfo(const EMaterialShaderType& type);

private:
    // EBlendType _blendType;
    std::map<EMaterialShaderType, MaterialShaderInfo> _shaderMap;
};

}