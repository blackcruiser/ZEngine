#pragma once

#include "CoreDefines.h"
#include "Shader.h"

class TEMaterial
{
public:
    TEMaterial();

    TEPtr<TEShader> GetShader(const EShaderType &type);

private:
    //EBlendType _blendType;
    std::map<EShaderType, TEPtr<TEShader>> _shaderMap;

};