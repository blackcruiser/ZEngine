#pragma once

#include "CoreDefines.h"
#include "BaseResource.h"

#include <filesystem>
#include <string>


namespace TE {

enum class EShaderStage : int
{
    Vertex = 0,
    Fragment
};

class ShaderResource : BaseResource
{
public:
    ShaderResource(EShaderStage stage, const std::filesystem::path& path);

    virtual void Load() override;
    virtual void Unload() override;

    const std::filesystem::path& GetSourcePath();
    const std::filesystem::path& GetBytecodePath();

    const std::vector<char>& GetByteCode();

private:
    EShaderStage _stage;
    std::filesystem::path _sourcePath, _bytecodePath;
    std::vector<char> _byteCode;
};

} // namespace TE
