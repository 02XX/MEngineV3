#include "IUUIDGenerator.hpp"
#include "UUID.hpp"

namespace MEngine::Core
{
class UUIDv7Generator : public IUUIDGenerator
{
  public:
    ~UUIDv7Generator() override = default;
    UUID Create() override;
};
} // namespace MEngine::Core