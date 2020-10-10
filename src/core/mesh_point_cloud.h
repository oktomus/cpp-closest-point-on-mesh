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

    inline void get_triangle(
        const std::size_t   idx,
        glm::vec3&          v1,
        glm::vec3&          v2,
        glm::vec3&          v3) const
    {
        // The index we receive is a vertex index.
        // Since each triangle is made of 3 vertex
        // and we don't share vertices in the point cloud,
        // we can deduce the v1, v2 and v3 easily.
        // TODO: once more points are added in the cloud
        // other than the vertices, this won't work anymore.
        if (idx % 3 == 0)
        {
            v1 = m_points[idx];
            v2 = m_points[idx + 1];
            v3 = m_points[idx + 2];
        }
        else if ((idx - 1) % 3 == 0)
        {
            v1 = m_points[idx - 1];
            v2 = m_points[idx];
            v3 = m_points[idx + 1];
        }
        else if ((idx - 2) % 3 == 0)
        {
            v1 = m_points[idx - 2];
            v2 = m_points[idx - 1];
            v3 = m_points[idx];
        }
        else
        {
            assert(false);
        }
    }

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
