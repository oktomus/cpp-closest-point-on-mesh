#pragma once

#include <glm/glm.hpp>

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
    Mesh(
        const std::vector<glm::vec3>&   vertices,
        const std::vector<std::size_t>& triangles);

    const std::vector<glm::vec3>& get_vertices() const;
    const std::vector<std::size_t>& get_triangles() const;

  private:
    const std::vector<glm::vec3> m_vertices;
    const std::vector<std::size_t> m_triangles;
};

} // namespace core