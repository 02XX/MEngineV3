#pragma once
#include <nlohmann/json_fwd.hpp>

using Json = nlohmann::json;
namespace MEngine
{
class IConfigure
{
  public:
    virtual ~IConfigure() = default;
    virtual const Json &GetJson() const = 0;
};
} // namespace MEngine
