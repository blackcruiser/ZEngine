#include "ShaderResource.h"

#define UNICODE
#define NOMINMAX
#include <Windows.h>

#include <fstream>

namespace TE {

ShaderResource::ShaderResource(EShaderStage stage, const std::filesystem::path& path) : _stage(stage), _sourcePath(path)
{
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


void ShaderResource::Load()
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    si.cb = sizeof(si);

    const std::filesystem::path shaderDirectoryPath = "../Engine/Shaders";

    std::wstring shaderStageDesc = _stage == EShaderStage::Vertex ? L"vertex" : L"fragment";
    std::filesystem::path shaderAbsolutePath = shaderDirectoryPath / _sourcePath;
    std::filesystem::path byteCodePath = std::filesystem::temp_directory_path() / (_sourcePath.stem() += ".spv");

    std::wstring arg = std::format(L"glslc.exe -fshader-stage={} -o {} {}", shaderStageDesc, byteCodePath.wstring(),
                                   shaderAbsolutePath.wstring());

    if (CreateProcess(NULL, arg.data(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    {
        WaitForSingleObject(pi.hProcess, INFINITE);

        DWORD exitCode;
        GetExitCodeProcess(pi.hProcess, &exitCode);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    if (std::filesystem::exists(byteCodePath))
        _bytecodePath = byteCodePath;
    else
        throw std::runtime_error("ShaderResource compile failed");

    _byteCode = readFile(_bytecodePath.string());

    _isLoaded = true;
}

void ShaderResource::Unload()
{
    _isLoaded = false;
}

const std::filesystem::path& ShaderResource::GetSourcePath()
{
    return _sourcePath;
}

const std::filesystem::path& ShaderResource::GetBytecodePath()
{
    return _bytecodePath;
}

const std::vector<char>& ShaderResource::GetByteCode()
{
    return _byteCode;
}

} // namespace TE