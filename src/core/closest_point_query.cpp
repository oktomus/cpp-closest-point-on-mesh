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

    static nanoflann::SearchParams search_params(
        32,    // ignored param
        0.0f,  // epsilon
        true); // true means we want results to be sorted by squared dist

    std::vector<std::size_t> ret_index(1);
    std::vector<float> out_dist_sqr(1);
    const std::size_t num_results =
        m_tree_index.knnSearch(
            glm::value_ptr(query_point),
            1, // We only want 1 result, the closest one.
            &ret_index[0],
            &out_dist_sqr[0]);

    if (num_results == 0)
        return false;

    glm::vec3 v1, v2, v3;
    m_mesh_point_cloud.get_triangle(ret_index[0], v1, v2, v3);

    result = closest_point_in_triangle(
        query_point,
        v1,
        v2,
        v3);

    const float distance2_to_triangle = distance2(result, query_point);

    if (distance2_to_triangle > max_distance)
        return false;;

    return true;
}

} // namespace core
