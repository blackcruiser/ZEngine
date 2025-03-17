#pragma once

#include <string>
#include <filesystem>


namespace TE {

enum class EShaderStage : int
{
    Vertex = 0,
    Fragment
};

class Shader
{
public:
    Shader();
    Shader(const std::filesystem::path& path);

    const std::filesystem::path& GetSourcePath();

    void SetBytecodePath(const std::filesystem::path& path);
    const std::filesystem::path& GetBytecodePath();

private:
    std::filesystem::path _sourcePath, _bytecodePath;
};

}