#pragma once
namespace MEngine::Function::Component
{
class MComponent
{

  public:
    virtual ~MComponent() = default;
    bool dirty = true;
};
} // namespace MEngine::Function::Component
