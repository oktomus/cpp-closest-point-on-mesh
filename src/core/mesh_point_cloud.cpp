#include "mesh_point_cloud.h"

namespace core
{

MeshPointCloud::MeshPointCloud(const Mesh& mesh)
  : m_mesh(mesh)
{
    // Add all mesh vertices in the point cloud.
    // TODO: Uniform sample the mesh to generate a denser cloud.
    const std::vector<Mesh::Vertex>& vertices = m_mesh.get_vertices();
    const std::size_t vertex_count = vertices.size();
    const std::vector<unsigned int>& triangles = m_mesh.get_triangles();
    assert(triangles.size() % 3 == 0);
    const std::size_t triangle_count = triangles.size() / 3;

    m_points.reserve(triangle_count * 3);

    for (std::size_t triangle_index = 0; triangle_index < triangle_count; ++triangle_index)
    {
        const std::size_t v1_index = triangles[triangle_index * 3];
        const std::size_t v2_index = triangles[triangle_index * 3 + 1];
        const std::size_t v3_index = triangles[triangle_index * 3 + 2];

        m_points.push_back(vertices[v1_index].pos);
        m_points.push_back(vertices[v2_index].pos);
        m_points.push_back(vertices[v3_index].pos);
    }
}

} // namespace core
