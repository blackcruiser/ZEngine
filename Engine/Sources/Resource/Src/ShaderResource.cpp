#include "ShaderResource.h"

#ifdef ZE_PLATFORM_WINDOWS
    #define UNICODE
    #define NOMINMAX
    #include <Windows.h>
#else
    #include <unistd.h>
    #include <sys/types.h>
#endif

#include <fstream>
#include <iostream>
#include <array>

namespace ZE {

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
    const std::filesystem::path shaderDirectoryPath = "../Engine/Shaders";
 
    std::filesystem::path shaderPath = shaderDirectoryPath / _sourcePath;
    std::filesystem::path absoluteShaderPath = std::filesystem::absolute(shaderPath);
    std::filesystem::path byteCodePath = std::filesystem::temp_directory_path() / (_sourcePath.stem() += ".spv");

#ifdef ZE_PLATFORM_WINDOWS
    std::wstring glslc = L"glslc";
    std::wstring shaderStageDesc = _stage == EShaderStage::Vertex ? L"vertex" : L"fragment";
    std::wstring arg = std::format(L" -fshader-stage={} -o {} {}", shaderStageDesc, byteCodePath.wstring(),
                                   absoluteShaderPath.wstring());
    std::wstring command = glslc + arg;

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    si.cb = sizeof(si);
    
    if (CreateProcess(NULL, command.data(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    {
        WaitForSingleObject(pi.hProcess, INFINITE);

        DWORD exitCode;
        GetExitCodeProcess(pi.hProcess, &exitCode);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
#else
    std::string glslc = "glslc";
    std::string shaderStageDesc = _stage == EShaderStage::Vertex ? "vertex" : "fragment";
    std::string arg = " -fshader-stage=" + shaderStageDesc + " -o " + byteCodePath.string() + " " + shaderAbsolutePath.string();
    
    std::array<char, 128> buffer;
    std::string result;

    std::string command = glslc + arg;
    FILE* pipe = popen(command.c_str(), "r");
    if (pipe != nullptr)
    {
        while (fgets(buffer.data(), 128, pipe) != NULL) 
            result += buffer.data();
            
        int returnCode = pclose(pipe);
        std::cout << returnCode << std::endl << result << std::endl;
    }
#endif

    if (std::filesystem::exists(byteCodePath))
    {
        _bytecodePath = byteCodePath;
        _byteCode = readFile(_bytecodePath.string());
    }
    else
        throw std::runtime_error("ShaderResource compile failed");

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

} // namespace ZE
