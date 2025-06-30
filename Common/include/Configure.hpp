#pragma once
#include "IConfigure.hpp"
#include <filesystem>
#include <nlohmann/json.hpp>
namespace MEngine
{
class Configure final : public IConfigure
{

  private:
    Json mJson;
    std::filesystem::path mPath = std::filesystem::current_path() / "appsettings.json";

  public:
    Configure();
    const Json &GetJson() const override;
};
} // namespace MEngine