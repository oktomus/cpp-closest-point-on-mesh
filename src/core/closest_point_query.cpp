#include "closest_point_query.h"

#include "math.h"

#include <chrono>
#include <cstddef>
#include <inttypes.h>
#include <iostream>
#include <limits>
#include <vector>

namespace core
{

ClosestPointQuery::ClosestPointQuery(const MeshPointCloud& mesh_point_cloud)
  : m_mesh_point_cloud(mesh_point_cloud)
{
    // Start a timer to know how long it takes to build the query object.
    auto timer_start = std::chrono::high_resolution_clock::now();

    auto timer_stop = std::chrono::high_resolution_clock::now();
    auto process_time = std::chrono::duration_cast<std::chrono::milliseconds>(timer_stop - timer_start).count();

    std::printf("Generated mesh query tree in %" PRId64 "ms.", process_time);
}

bool ClosestPointQuery::get_closest_point(
    const glm::vec3&    query_point,
    float               max_distance,
    glm::vec3&          result) const
{
    assert(max_distance > 0.0f);

    const std::size_t triangle_count = m_mesh_point_cloud.kdtree_get_point_count() / 3;

    //=> Naive implementation.
    // 1. Go through each triangle
    // 2. Compute the closest point on a particular mesh
    // 3. Keep the closest
    float closest_distance2 = std::numeric_limits<float>::max();
    result = { 0, 0, 0 };
    bool found = false;

    for (std::size_t triangle_index = 0; triangle_index < triangle_count; ++triangle_index)
    {
        const glm::vec3& v1 = m_mesh_point_cloud.kdtree_get_pt(triangle_index * 3);
        const glm::vec3& v2 = m_mesh_point_cloud.kdtree_get_pt(triangle_index * 3 + 1);
        const glm::vec3& v3 = m_mesh_point_cloud.kdtree_get_pt(triangle_index * 3 + 2);

        bool success = false;
        const glm::vec3 p = closest_point_in_triangle(
            query_point,
            v1,
            v2,
            v3);

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
