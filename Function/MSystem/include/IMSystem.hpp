#pragma once

namespace MEngine::Function::System
{

class IMSystem
{
  public:
    virtual ~IMSystem() = default;
    virtual void Init() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Shutdown() = 0;
};

} // namespace MEngine::Function::System