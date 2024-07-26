#include "FilePathHandler.hpp"

#include <filesystem>
#include <iostream>

std::string GetProjectPath()
{
    return std::filesystem::current_path().string();
}

std::string GetShaderDirPath()
{
    return GetProjectPath() + "\\Shaders";
}

std::string GetTexturesDirPath()
{
    return GetProjectPath() + "\\Textures";
}

std::string GetBlockTexturesDirPath()
{
    return GetTexturesDirPath() + "\\BlockTextures";
}

std::string GetUITexturesDirPath()
{
    return GetTexturesDirPath() + "\\UITextures";
}