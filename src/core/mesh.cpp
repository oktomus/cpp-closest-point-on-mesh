#include "mesh.h"

namespace core
{

Mesh::Mesh(
    const std::vector<glm::vec3>&   vertices,
    const std::vector<std::size_t>& triangles)
  : m_vertices(vertices)
  , m_triangles(triangles)
{ }

const std::vector<glm::vec3>& Mesh::get_vertices() const
{
    return m_vertices;
}

const std::vector<std::size_t>& Mesh::get_triangles() const
{
    return m_triangles;
}

} // namespace core