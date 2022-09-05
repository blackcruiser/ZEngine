#include "TextureResource.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace TE {

TextureResource::TextureResource(const std::filesystem::path& path) : _sourcePath(path), _data(nullptr)
{
}

TextureResource::~TextureResource()
{
}

void TextureResource::Load()
{
    int width, height, channels;
    _data = stbi_load(_sourcePath.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);
    _width = static_cast<uint32_t>(width);
    _height = static_cast<uint32_t>(height);
    _channels = static_cast<uint32_t>(channels);

    _isLoaded = true;
}

void TextureResource::Unload()
{
    _isLoaded = false;

    if (_data != nullptr)
    {
        stbi_image_free(_data);
        _data = nullptr;
    }
}

const std::filesystem::path& TextureResource::GetSourcePath()
{
    return _sourcePath;
}

uint32_t TextureResource::GetWidth()
{
    return _width;
}

uint32_t TextureResource::GetHeight()
{
    return _height;
}

uint32_t TextureResource::GetChannels()
{
    return _channels;
}

const void* TextureResource::GetData()
{
    return _data;
}

} // namespace TE