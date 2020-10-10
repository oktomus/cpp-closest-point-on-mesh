#include "closest_point_query.h"

#include "math.h"

#include <cstddef>
#include <limits>
#include <vector>

namespace core
{

ClosestPointQuery::ClosestPointQuery(const Mesh& mesh)
  : m_mesh(mesh)
{}

bool ClosestPointQuery::get_closest_point(
    const glm::vec3&    query_point,
    float               max_distance,
    glm::vec3&          result) const
{
    assert(max_distance > 0.0f);

    const std::vector<Mesh::Vertex>& vertices = m_mesh.get_vertices();
    const std::size_t vertex_count = vertices.size();
    const std::vector<unsigned int>& triangles = m_mesh.get_triangles();
    assert(triangles.size() % 3 == 0);
    const std::size_t triangle_count = triangles.size() / 3;

    //=> Naive implementation.
    // 1. Go through each triangle
    // 2. Compute the closest point on a particular mesh
    // 3. Keep the closest
    float closest_distance2 = std::numeric_limits<float>::max();
    result = { 0, 0, 0 };
    bool found = false;

    for (std::size_t triangle_index = 0; triangle_index < triangle_count; ++triangle_index)
    {
        const std::size_t v1_index = triangles[triangle_index * 3];
        const std::size_t v2_index = triangles[triangle_index * 3 + 1];
        const std::size_t v3_index = triangles[triangle_index * 3 + 2];

        const glm::vec3& v1 = vertices[v1_index].pos;
        const glm::vec3& v2 = vertices[v2_index].pos;
        const glm::vec3& v3 = vertices[v3_index].pos;

        const glm::vec3 p = (v1 + v2 + v3) * 0.33333f;

        const float distance2_to_triangle = distance2(p, query_point);

        if (distance2_to_triangle > max_distance)
            continue;

        if (distance2_to_triangle < closest_distance2)
        {
            closest_distance2 = distance2_to_triangle;
            result = p;
            found = true;
        }
    }

    return found;
}

} // namespace core
