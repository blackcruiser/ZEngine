#pragma once

#include <stb_image.h>
#include <vulkan/vulkan.h>

#include <filesystem>
#include <string>


namespace TE {

class Texture
{
public:
    Texture();
    Texture(const std::filesystem::path& path);
    virtual ~Texture();

    bool IsLoaded();
    void Load();
    void Unload();

    const std::filesystem::path& GetSourcePath();
    uint32_t GetWidth();
    uint32_t GetHeight();
    uint32_t GetChannels();

    stbi_uc* GetData();

private:
    std::filesystem::path _sourcePath;

    uint32_t _width, _height, _channels;
    stbi_uc* _data;
};

} // namespace TE