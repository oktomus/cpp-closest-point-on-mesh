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

    const std::vector<Mesh::Vertex>& vertices = m_mesh.get_vertices();
    const std::vector<unsigned int>& triangles = m_mesh.get_triangles();

    m_vertex_count = vertices.size();
    m_triangle_count = triangles.size();

    // Send vertices (pos + normal).
    glGenBuffers(1, &m_vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, m_vertex_count * sizeof(Mesh::Vertex), vertices.data(), GL_STATIC_DRAW);

    // Send triangles.
    GLuint ebo_id;
    glGenBuffers(1, &ebo_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_triangle_count * sizeof(unsigned int), triangles.data(), GL_STATIC_DRAW);

    // Define the offset and stride for positions.
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (void*)0);

    // Define the offset and stride for normals.
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (void*) offsetof(Mesh::Vertex, normal));

    glBindVertexArray(0);
}

void RasterizedMesh::draw() const
{
    assert(m_vao_id != 0 && m_triangle_count != 0);
    glBindVertexArray(m_vao_id);
    glDrawElements(m_draw_mode, m_triangle_count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

} // namespace core
