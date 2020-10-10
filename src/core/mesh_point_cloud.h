#pragma once

#include "mesh.h"

#include <nanoflann/nanoflann.hpp>
#include <glm/glm.hpp>

#include <cstdint>
#include <vector>

namespace core
{

/**
 * @brief Create a point cloud using a mesh.
 * TODO: Allow to generate points on the mesh to
 * increase precision.
 */
class MeshPointCloud
{
  public:
    MeshPointCloud(const Mesh& mesh);

    // nanoflann compatibility implementaiton.
    inline std::size_t kdtree_get_point_count() const
    {
        return m_points.size();
    }

    inline float kdtree_get_pt(const std::size_t idx, int dim) const
    {
        if (dim == 0)
            return m_points[idx].x;
        else if (dim == 1)
            return m_points[idx].y;
        else
            return m_points[idx].z;
    }

    inline glm::vec3 kdtree_get_pt(const std::size_t idx) const
    {
        return m_points[idx];
    }

    template<class BBOX>
    bool kdtree_get_bbox(BBOX&) const
    { return false; }

  private:
    const Mesh&             m_mesh;
    std::vector<glm::vec3>  m_points;
};

} // namespace core
