#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>

#include <cstddef>
#include <vector>

namespace core
{

/**
 * @brief Mesh object.
 *
 * Simply store vertices and triangles.
 */
class Mesh
{
  public:
    struct Vertex
    {
      glm::vec3 pos;
      glm::vec3 normal;
    };

    Mesh(
        const std::vector<Vertex>& vertices,
        const std::vector<unsigned int>& triangles);

    const std::vector<Vertex>& get_vertices() const;
    const std::vector<unsigned int>& get_triangles() const;

  private:
    const std::vector<Vertex> m_vertices;
    const std::vector<unsigned int> m_triangles;
};

} // namespace core
