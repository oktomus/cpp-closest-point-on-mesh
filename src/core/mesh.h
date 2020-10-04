#pragma once

#include <glm/glm.hpp>

#include <GL/gl3w.h>

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
        const std::vector<GLuint>& triangles);

    const std::vector<glm::vec3>& get_vertices() const;
    const std::vector<GLuint>& get_triangles() const;

  private:
    const std::vector<glm::vec3> m_vertices;
    const std::vector<GLuint> m_triangles;
};

} // namespace core