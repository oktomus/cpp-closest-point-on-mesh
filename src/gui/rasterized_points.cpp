#include "rasterized_points.h"

#include <cassert>

namespace gui
{

RasterizedPoints::RasterizedPoints()
  : m_vao_id(0)
  , m_vbo_id(0)
{
}

void RasterizedPoints::set_points(const std::vector<RasterizedPoints::Point>& points)
{
    // Declare the buffers if required.
    if (m_vao_id == 0)
        glGenVertexArrays(1, &m_vao_id);
    if (m_vbo_id == 0)
        glGenBuffers(1, &m_vbo_id);

    assert(m_vao_id > 0);
    assert(m_vbo_id > 0);

    m_point_count = points.size();

    // First, send the data on the GPU.
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, m_point_count * sizeof(RasterizedPoints::Point), points.data(), GL_DYNAMIC_DRAW);

    // Then, explain to the GPU how things are stored.
    // Define the offset and stride for positions.
    glBindVertexArray(m_vao_id);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(RasterizedPoints::Point), (void*)0);

    // Define the offset and stride for colors.
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(RasterizedPoints::Point), (void*) offsetof(RasterizedPoints::Point, color));

    glBindVertexArray(0);
}

void RasterizedPoints::draw() const
{
    if (m_point_count == 0) return;

    assert(m_vao_id > 0);

    glBindVertexArray(m_vao_id);
    glDrawArrays(GL_POINTS, 0, m_point_count);
    glBindVertexArray(0);
}

} // namespace gui