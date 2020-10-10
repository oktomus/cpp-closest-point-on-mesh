#pragma once

#include "mesh.h"

#include <glm/glm.hpp>

namespace core
{

class ClosestPointQuery
{
  public:
    ClosestPointQuery(const Mesh& mesh);

    /**
     * @brief Return the closest point on the mesh within the specified maximum search distance.
     */
    bool get_closest_point(
        const glm::vec3&    query_point,
        float               max_distance,
        glm::vec3&          result) const;

  private:
    const Mesh& m_mesh;
};

} // namespace core
