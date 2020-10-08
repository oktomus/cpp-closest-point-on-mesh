#pragma once

#include "mesh.h"

#include <glad/glad.h>

#include <cstddef>

namespace core
{

/**
 * @brief Mesh wrapper that can be rendered with OpenGL.
 */
class RasterizedMesh
{
  public:
    RasterizedMesh(const Mesh& mesh);

    /**
     * @brief Prepare the mesh for OpenGL rendering.
     */
    void prepare();

    void draw() const;

  private:
    const Mesh& m_mesh;

    std::size_t m_vertex_count;
    std::size_t m_triangle_count;

    /**
     * @brief Mode used to draw.
     * GL_TRIANGLES, QUADS, ...
     */
    GLenum m_draw_mode;

    /**
     * @brief Vertex Array Object id.
     */
    GLuint m_vao_id;

    /**
     * @brief Vertex buffer object id
     */
    GLuint m_vbo_id;
};

} // namespace core