#pragma once

#include <glm/glm.hpp>

// Forward declarations.
class GLFWwindow;

namespace gui 
{

class OrbitCamera
{
  public:
    OrbitCamera();

    glm::vec3 get_position() const;

    glm::mat4 projection(const int screen_width, const int screen_height) const;
    glm::mat4 view() const;
    glm::vec3 up() const;

    void glfw_process_mouse_move(double xpos, double ypos, float delta_time);
    void glfw_process_mouse_action(int button, int action, int mods, float delta_time);
    void glfw_process_scroll(double xoffset, double yoffset, float delta_time);

  private:
    glm::vec3   m_position;
    glm::vec3   m_target;
    glm::vec3   m_camera_pivot_rotation;
    float       m_distance_to_target;
    float       m_fov;
    bool        m_rotate_when_mouse_move;
    bool        m_pan_when_mouse_move;
    float       m_last_mouse_pos_x;
    float       m_last_mouse_pos_y;

    void compute_position();
};
    
} // gui
