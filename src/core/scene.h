#pragma once

#include "mesh.h"
#include "rasterized_mesh.h"

#include <vector>

namespace core
{

/**
 * @brief Store multiple meshes and everything required to render them.
 */
class Scene
{
  public:
    Scene(const std::vector<Mesh>& meshes);

    std::size_t get_mesh_count() const;

    const Mesh& get_mesh(const std::size_t index) const;

    void render() const;

  private:
    const std::vector<Mesh>     m_meshes;
    std::vector<RasterizedMesh> m_drawing_meshes;
};

} // namespace core
