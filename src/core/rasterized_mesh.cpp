#include "rasterized_mesh.h"

#include "mesh.h"

#include <cassert>
#include <cstddef>

namespace core
{

RasterizedMesh::RasterizedMesh(const Mesh& mesh)
  : m_mesh(mesh)
  , m_draw_mode(GL_TRIANGLES)
  , m_vao_id(0)
  , m_vbo_id(0)
{}

void RasterizedMesh::prepare()
{
    glGenVertexArrays(1, &m_vao_id);
    glBindVertexArray(m_vao_id);

    const std::vector<glm::vec3>& vertices = m_mesh.get_vertices();
    const std::vector<std::size_t>& triangles = m_mesh.get_triangles();

    m_vertex_count = vertices.size();
    m_triangle_count = triangles.size();

    // Send vertices.
    glGenBuffers(1, &m_vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, m_vertex_count * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    // Send triangles.
    GLuint ebo_id;
    glGenBuffers(1, &ebo_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_triangle_count * sizeof(std::size_t), triangles.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    glBindVertexArray(0);
}

void RasterizedMesh::draw() const
{
    glBindVertexArray(m_vao_id);
    glDrawElements(m_draw_mode, m_triangle_count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

} // namespace core