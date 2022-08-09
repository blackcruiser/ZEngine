#pragma once

#include <string>


namespace TE {

enum class EShaderType
{
    Vertex = 0,
    Fragment
};

class Shader
{
public:
    Shader(const std::string& path);

    const std::string& GetPath();

private:
    std::string _path;
};

}