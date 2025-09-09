#pragma once
#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>
namespace MEngine::Core
{
class Vertex
{
  public:
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
    static std::vector<vk::VertexInputAttributeDescription> GetVertexInputAttributeDescription();
    static vk::VertexInputBindingDescription GetVertexInputBindingDescription();
};
} // namespace MEngine::Core