#include "scene.h"

#include "mesh.h"
#include "rasterized_mesh.h"

namespace core
{

Scene::Scene(const std::vector<Mesh>& meshes)
  : m_meshes(meshes)
{ 
    // Prepare to render meshes.
    m_drawing_meshes.reserve(m_meshes.size());

    for(const Mesh& mesh : m_meshes)
    {
        RasterizedMesh drawing_mesh = RasterizedMesh(mesh);
        drawing_mesh.prepare();
        m_drawing_meshes.push_back(drawing_mesh);
    }
}

void Scene::render() const
{
    for(const RasterizedMesh& drawing_mesh : m_drawing_meshes)
    {
        drawing_mesh.draw();
    }
}

} // namespace core