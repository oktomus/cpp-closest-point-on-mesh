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

    void glfw_process_mouse_move(GLFWwindow* window, double xpos, double ypos);
    void glfw_process_mouse_action(GLFWwindow* window, int button, int action, int mods);
    void glfw_process_scroll(GLFWwindow* window, double xoffset, double yoffset);

  private:
    glm::vec3   m_position;
    glm::vec3   m_target;
    float       m_fov;
    bool        m_rotate_when_mouse_move;
    float       m_last_mouse_pos_x;
    float       m_last_mouse_pos_y;
};
    
} // gui
