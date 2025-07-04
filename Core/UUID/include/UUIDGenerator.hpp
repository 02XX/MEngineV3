#include "IUUIDGenerator.hpp"
#include "UUID.hpp"

namespace MEngine::Core
{
class UUIDGenerator : public IUUIDGenerator
{
  public:
    ~UUIDGenerator() override = default;
    UUID Create() override;
};
} // namespace MEngine::Core