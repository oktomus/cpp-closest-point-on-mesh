#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <cstddef>
#include <vector>

namespace gui
{

/**
 * @brief OpenGL drawing implementation for a list of point.
 * Give it a list of point+color and it will do the necessary to draw it.
 */
class RasterizedPoints
{
  public:
    struct Point
    {
        glm::vec3 pos;
        glm::vec4 color;
    };

    RasterizedPoints();

    /**
     * @brief Set the points that needs to be rasterized.
     */
    void set_points(const std::vector<Point>& points);

    void draw() const;

  private:
    std::size_t m_point_count;

    /**
     * @brief Vertex Array Object id.
     */
    GLuint m_vao_id;

    /**
     * @brief Vertex buffer object id
     */
    GLuint m_vbo_id;
};

} // namespace gui