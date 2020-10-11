#include "mesh_point_cloud.h"

namespace core
{

MeshPointCloud::MeshPointCloud(const Mesh& mesh)
  : m_mesh(mesh)
{
    // Add all mesh vertices in the point cloud.
    const std::vector<Mesh::Vertex>& vertices = m_mesh.get_vertices();
    const std::vector<unsigned int>& triangles = m_mesh.get_triangles();
    const std::size_t index_count = triangles.size();
    assert(index_count % 3 == 0);

    m_points.reserve(index_count);

    for (std::size_t i = 0; i < index_count; ++i)
    {
        m_points.push_back(vertices[triangles[i]].pos);
    }
}

} // namespace core
