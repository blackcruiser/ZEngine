#pragma once

#include "BaseResource.h"

#include <vulkan/vulkan.h>

#include <filesystem>
#include <string>


namespace ZE {

class TextureResource : public BaseResource
{
public:
    TextureResource(const std::filesystem::path& path);
    virtual ~TextureResource();

    virtual void Load() override;
    virtual void Unload() override;

    const std::filesystem::path& GetSourcePath();
    uint32_t GetWidth();
    uint32_t GetHeight();
    uint32_t GetChannels();

    const void* GetData();

private:
    std::filesystem::path _sourcePath;

    uint32_t _width, _height, _channels;
    void* _data;
};

} // namespace ZE