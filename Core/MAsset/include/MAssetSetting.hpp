#pragma once
#include <filesystem>
#include <vector>

namespace MEngine::Core::Asset
{

class MAssetSetting
{
  protected:
    std::vector<std::string> supportedExtensions{};

  public:
    std::string name;
    std::filesystem::path assetPath;

  public:
    MAssetSetting() = default;
    virtual ~MAssetSetting() = default;
};
} // namespace MEngine::Core::Asset