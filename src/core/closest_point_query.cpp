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

    // nanoflann (the KDTree library) use squared distance. We do the same.
    const float max_distance2 = max_distance * max_distance;

    // Define how many points we take from the tree to find the closest point on the mesh.
    // A number that is too low will generate incorrect results when the mesh density is high.
    // The points we process are the nearest points in the cloud to the query point.
    const std::size_t point_to_process_max_count = 2000;
    std::vector<std::size_t> ret_index(point_to_process_max_count);
    std::vector<float> out_dist_sqr(point_to_process_max_count);

    // Use the KDTree to find the nearest points to the `query_point`.
    const std::size_t num_results =
        m_tree_index.knnSearch(
            glm::value_ptr(query_point),
            point_to_process_max_count,
            &ret_index[0],
            &out_dist_sqr[0]);

    bool found = false;
    float closest_distance2 = 0.0f;

    // Find the closest point on the mesh using all points near to `query_point`.
    // To do so, we use the triangle on which each point is and compute the closest
    // point to query_point that is on the triangle.
    // For all these "triangles points", we keep the closest one to the query point.
    // FIXME: We analyse the same triangles multiple times.
    for (std::size_t i = 0; i < num_results; ++i)
    {
        // Ask to the point cloud which triangle is this point on.
        glm::vec3 v1, v2, v3;
        m_mesh_point_cloud.get_triangle(ret_index[i], v1, v2, v3);

        // Compute the closest point to `query_point` that is on the triangle.
        glm::vec3 p = closest_point_in_triangle(
            query_point,
            v1,
            v2,
            v3);

        // From all triangles, keep the closest one.
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
