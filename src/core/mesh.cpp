#include "mesh.h"

namespace core
{

Mesh::Mesh(
    const std::vector<Mesh::Vertex>&    vertices,
    const std::vector<unsigned int>&    triangles)
  : m_vertices(vertices)
  , m_triangles(triangles)
{ }

const std::vector<Mesh::Vertex>& Mesh::get_vertices() const
{
    return m_vertices;
}

const std::vector<unsigned int>& Mesh::get_triangles() const
{
    return m_triangles;
}

} // namespace core
