#pragma once

#include "mesh_point_cloud.h"

#include <nanoflann/nanoflann.hpp>
#include <glm/glm.hpp>

#include <vector>

namespace core
{

/**
 * @brief Fast closest point to mesh.
 *
 * After creating the query object, just call
 * `get_closest_point`.
 *
 * When creating the query object, a tree will
 * be generated to speed up look up time.
 */
class ClosestPointQuery
{
  public:
    ClosestPointQuery(const MeshPointCloud& mesh_point_cloud);

    /**
     * @brief Return the closest point on the mesh within the specified maximum search distance.
     */
    bool get_closest_point(
        const glm::vec3&    query_point,
        float               max_distance,
        glm::vec3&          result) const;

  private:
    const MeshPointCloud& m_mesh_point_cloud;

    // nanoflann kdtree. Will be used to speed up look up time.
    typedef nanoflann::KDTreeSingleIndexAdaptor<
        nanoflann::L2_Simple_Adaptor<float, MeshPointCloud>,
        MeshPointCloud,
        3, /* Go 3D! */
        size_t> TreeIndex;
    TreeIndex m_tree_index;

    typedef std::vector<std::pair<std::size_t, float>> TreeSearchVectorResult;
};

} // namespace core
