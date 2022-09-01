#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace TE {

Texture::Texture()
{
}

Texture::Texture(const std::filesystem::path& path)
    : _sourcePath(path), _data(nullptr)
{
}

Texture::~Texture()
{
}

bool Texture::IsLoaded()
{
    return _data != nullptr;
}

void Texture::Load()
{
    int width, height, channels;
    _data = stbi_load(_sourcePath.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);
    _width = static_cast<uint32_t>(width);
    _height = static_cast<uint32_t>(height);
    _channels = static_cast<uint32_t>(channels);
}

void Texture::Unload()
{
    if (_data != nullptr)
    {
        stbi_image_free(_data);
        _data = nullptr;
    }
}

const std::filesystem::path& Texture::GetSourcePath()
{
    return _sourcePath;
}

uint32_t Texture::GetWidth()
{
    return _width;
}

uint32_t Texture::GetHeight()
{
    return _height;
}

uint32_t Texture::GetChannels()
{
    return _channels;
}

stbi_uc* Texture::GetData()
{
    return _data;
}

} // namespace TE