#include "mesh_point_cloud.h"

#include <chrono>
#include <iostream>

namespace core
{

MeshPointCloud::MeshPointCloud(const Mesh& mesh)
  : m_mesh(mesh)
{
    // Start a timer to know how long it takes to generate the point cloud.
    auto timer_start = std::chrono::high_resolution_clock::now();

    // Add all mesh vertices in the point cloud.
    const std::vector<Mesh::Vertex>& vertices = m_mesh.get_vertices();
    const std::vector<unsigned int>& triangles = m_mesh.get_triangles();
    const std::size_t index_count = triangles.size();
    assert(index_count % 3 == 0);

    std::cout << "Creating a new mesh point cloud...\n";
    std::cout << "\tMesh vertex count: " << vertices.size() << "\n";
    std::cout << "\tMesh triangle count: " << (index_count / 3) << "\n";

    m_points.reserve(index_count);

    for (std::size_t i = 0; i < index_count; ++i)
    {
        m_points.push_back(vertices[triangles[i]].pos);
    }

    auto timer_stop = std::chrono::high_resolution_clock::now();
    auto process_time = std::chrono::duration_cast<std::chrono::milliseconds>(timer_stop - timer_start).count();

    std::cout << "Generated mesh point cloud in " << process_time << "ms.\n";
    std::cout << "\tPoint count: " << m_points.size() << "\n";
}

} // namespace core
