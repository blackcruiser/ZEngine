#include "RSMaterial.h"
#include "Graphic/VulkanImage.h"
#include "Graphic/VulkanShader.h"
#include "Render/Shader.h"
#include "Render/Texture.h"
#include "Scene/MaterialComponent.h"

#define UNICODE
#define NOMINMAX
#include <Windows.h>

#include <filesystem>
#include <format>
#include <fstream>

namespace TE {

RSMaterial::RSMaterial(TPtr<Device> device, TPtr<MaterialComponent> materialComponent)
    : _device(device), _owner(materialComponent), vkPipeline(VK_NULL_HANDLE), vkPipelineLayout(VK_NULL_HANDLE),
      descriptorSetLayout(VK_NULL_HANDLE), descriptorSet(VK_NULL_HANDLE), _uniformBuffer(VK_NULL_HANDLE),
      _uniformBufferMemory(VK_NULL_HANDLE)
{
}

RSMaterial::~RSMaterial()
{
    if (vkPipeline != VK_NULL_HANDLE)
        _device->DestroyPipeline(vkPipeline);

    if (vkPipelineLayout != VK_NULL_HANDLE)
        _device->DestroyPipelineLayout(vkPipelineLayout);

    if (descriptorSetLayout != VK_NULL_HANDLE)
        _device->DestroyDescriptorSetLayout(descriptorSetLayout);

    //            if (descriptorSet != VK_NULL_HANDLE)
    //_device->FreeDescriptorSet(descriptorSet);

    if (_uniformBufferMemory != VK_NULL_HANDLE)
        _device->FreeMemory(_uniformBufferMemory);

    if (_uniformBuffer != VK_NULL_HANDLE)
        _device->DestroyBuffer(_uniformBuffer);
}

TPtr<VulkanImage> CreateGraphicImage(TPtr<Device> device, TPtr<CommandPool> commandPool, TPtr<Texture> texture)
{
    assert(texture->IsLoaded());

    TPtr<VulkanImage> vulkanImage =
        std::make_shared<VulkanImage>(device, texture->GetWidth(), texture->GetHeight(), VK_FORMAT_R8G8B8A8_SRGB);

    uint32_t imageSize = texture->GetWidth() * texture->GetHeight() * 4;
    vulkanImage->UploadData(commandPool, texture->GetData(), imageSize);

    return vulkanImage;
}

void RSMaterial::CreateGraphicTextures(TPtr<Device> device, TPtr<CommandPool> commandPool)
{
    assert(_owner.expired() == false);

    TPtr<MaterialComponent> materialComponent = _owner.lock();

    const std::unordered_map<EShaderStage, std::list<TextureBindingInfo>>& textureMap =
        materialComponent->GetTextureMap();

    for (auto& [stage, textureList] : textureMap)
    {
        if (textureList.empty() == true)
            continue;

        std::list<VulkanImageBindingInfo> vulkanBindingInfoList;
        for (const TextureBindingInfo& bindingInfo : textureList)
        {
            VulkanImageBindingInfo vulkanBindingInfo;

            vulkanBindingInfo.bindingPoint = bindingInfo.bindingPoint;
            vulkanBindingInfo.vulkanImage = CreateGraphicImage(device, commandPool, bindingInfo.texture);

            vulkanBindingInfoList.push_back(vulkanBindingInfo);
        }
        _textures.insert(std::make_pair(stage, vulkanBindingInfoList));
    }
}

static std::vector<char> readFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

TPtr<VulkanShader> CreateGraphicShader(TPtr<Device> device, EShaderStage shaderStage, TPtr<Shader> shader)
{
    if (shader == nullptr)
        return nullptr;

    if (shader->GetBytecodePath().empty())
    {
        STARTUPINFO si;
        PROCESS_INFORMATION pi;

        ZeroMemory(&si, sizeof(si));
        ZeroMemory(&pi, sizeof(pi));
        si.cb = sizeof(si);

        const std::filesystem::path shaderDirectoryPath = "./Engine/Shaders";

        std::wstring shaderStageDesc = shaderStage == EShaderStage::Vertex ? L"vertex" : L"fragment";
        std::filesystem::path shaderPath = shaderDirectoryPath / shader->GetSourcePath();
        std::filesystem::path byteCodePath =
            std::filesystem::temp_directory_path() / (shader->GetSourcePath().stem() += ".spv");

        std::wstring arg = std::format(L"glslc.exe -fshader-stage={} -o {} {}", shaderStageDesc, byteCodePath.wstring(),
                                       shaderPath.wstring());

        if (CreateProcess(NULL, arg.data(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
        {
            WaitForSingleObject(pi.hProcess, INFINITE);

            DWORD exitCode;
            GetExitCodeProcess(pi.hProcess, &exitCode);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }

        if (std::filesystem::exists(byteCodePath))
            shader->SetBytecodePath(byteCodePath);
        else
            throw std::runtime_error("Shader compile failed");
    }

    std::vector<char> byteCode = readFile(shader->GetBytecodePath().string());
    return std::make_shared<VulkanShader>(device, byteCode);
}

void RSMaterial::CreateGraphicShaders(TPtr<Device> device)
{
    assert(_owner.expired() == false);
    TPtr<MaterialComponent> materialComponent = _owner.lock();

    const TPtrUnorderedMap<EShaderStage, Shader>& shaderMap = materialComponent->GetShaderMap();
    for (auto& [stage, shader] : shaderMap)
    {
        TPtr<VulkanShader> vulkanShader = CreateGraphicShader(device, stage, shader);
        _shaders.insert(std::make_pair(stage, vulkanShader));
    }
}

TPtr<VulkanShader> RSMaterial::GetGraphicShader(EShaderStage stage)
{
    if (_shaders.find(stage) == _shaders.end())
        return nullptr;
    else
        return _shaders[stage];
}

std::optional<std::reference_wrapper<std::list<VulkanImageBindingInfo>>> RSMaterial::GetGraphicTexture(
    EShaderStage stage)
{
    if (_textures.find(stage) == _textures.end())
        return std::optional<std::reference_wrapper<std::list<VulkanImageBindingInfo>>>();
    else
        return _textures[stage];
}

} // namespace TE
