#include "Shader.h"


namespace TE {

Shader::Shader()
{
}

Shader::Shader(const std::filesystem::path& path) : _sourcePath(path)
{
}

const std::filesystem::path& Shader::GetSourcePath()
{
    return _sourcePath;
}

void Shader::SetBytecodePath(const std::filesystem::path& path)
{
    _bytecodePath = path;
}

const std::filesystem::path& Shader::GetBytecodePath()
{
    return _bytecodePath;
}

}