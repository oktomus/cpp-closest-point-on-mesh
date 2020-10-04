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

    void render() const;

  private:
    const std::vector<Mesh>     m_meshes;
    std::vector<RasterizedMesh> m_drawing_meshes;
};

} // namespace core