#include <string>
#include <filesystem>
#include <iostream>

#include "FilePathHandler.hpp"

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