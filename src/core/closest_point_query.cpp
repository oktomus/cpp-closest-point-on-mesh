#include "closest_point_query.h"

#include "math.h"

#include <glm/gtc/type_ptr.hpp>

#include <chrono>
#include <cstddef>
#include <iostream>
#include <limits>
#include <vector>

namespace core
{

ClosestPointQuery::ClosestPointQuery(const MeshPointCloud& mesh_point_cloud)
  : m_mesh_point_cloud(mesh_point_cloud)
  , m_tree_index(
    3,
    mesh_point_cloud,
    nanoflann::KDTreeSingleIndexAdaptorParams(10 /* tree leaf max size */))
{
    // Start a timer to know how long it takes to build the query object.
    auto timer_start = std::chrono::high_resolution_clock::now();

    m_tree_index.buildIndex();

    auto timer_stop = std::chrono::high_resolution_clock::now();
    auto process_time = std::chrono::duration_cast<std::chrono::milliseconds>(timer_stop - timer_start).count();

    std::cout << "Generated mesh query tree in " << process_time << "ms.\n";
}

bool ClosestPointQuery::get_closest_point(
    const glm::vec3&    query_point,
    float               max_distance,
    glm::vec3&          result) const
{
    assert(max_distance > 0.0f);

    const float max_distance2 = max_distance * max_distance;

    // Take the 10 closest and keep the best out of them.
    const std::size_t expected_result_count = 200;
    std::vector<std::size_t> ret_index(expected_result_count);
    std::vector<float> out_dist_sqr(expected_result_count);
    const std::size_t num_results =
        m_tree_index.knnSearch(
            glm::value_ptr(query_point),
            expected_result_count,
            &ret_index[0],
            &out_dist_sqr[0]);

    bool found = false;
    float closest_distance2 = 0.0f;

    for (std::size_t i = 0; i < num_results; ++i)
    {
        // Ask the triangle to the point cloud.
        glm::vec3 v1, v2, v3;
        m_mesh_point_cloud.get_triangle(ret_index[i], v1, v2, v3);

        // Compute the point on the triangle.
        glm::vec3 p = closest_point_in_triangle(
            query_point,
            v1,
            v2,
            v3);

        // Keep the best.
        const float distance2_to_triangle = distance2(p, query_point);

        if (distance2_to_triangle < max_distance2
            && (!found || distance2_to_triangle < closest_distance2))
        {
            found = true;
            result = p;
            closest_distance2 = distance2_to_triangle;
        }
    }

    return found;
}

} // namespace core
