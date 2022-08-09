#include "Shader.h"


namespace TE {

Shader::Shader(const std::string& path) : _path(path)
{
}

const std::string& Shader::GetPath()
{
    return _path;
}

}