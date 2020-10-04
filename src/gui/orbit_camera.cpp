#include "orbit_camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <GLFW/glfw3.h>

namespace gui
{

OrbitCamera::OrbitCamera()
{
    m_position = { 0, 30, 30 };
    m_target = { 0, 0, 0 };
    m_fov = 45.0f;
}

glm::vec3 OrbitCamera::get_position() const
{
    return m_position;
}

glm::mat4 OrbitCamera::projection(const int screen_width, const int screen_height) const
{
    return glm::perspective(
        glm::radians(m_fov), 
        (float) screen_width / (float)screen_height, 
        0.1f, 1000.0f);  
}

glm::mat4 OrbitCamera::view() const
{
    glm::vec3 direction = glm::normalize(m_position - m_target);
    glm::vec3 up = glm::vec3(0, 1, 0);
    glm::vec3 right = glm::normalize(glm::cross(up, direction));
    glm::vec3 camera_up = glm::cross(direction, right);
    return glm::lookAt(m_position, direction, camera_up);
}

glm::vec3 OrbitCamera::up() const
{
    glm::vec3 direction = glm::normalize(m_position - m_target);
    glm::vec3 camera_up = glm::vec3(0, 1, 0);
    glm::vec3 right = glm::normalize(glm::cross(camera_up, direction));
    return glm::cross(direction, right);
}

void OrbitCamera::glfw_process_mouse_move(double xpos, double ypos, float delta_time)
{
    if (m_rotate_when_mouse_move)
    {
        float delta_x = xpos - m_last_mouse_pos_x;
        float delta_y = ypos - m_last_mouse_pos_y;

        float x_weight = -std::abs(delta_x * 0.3f);
        float y_weight = -std::abs(delta_y * 0.3f);

        m_position = glm::rotate(m_position, delta_x * delta_time * x_weight, glm::vec3(0, 1, 0));
        m_position = glm::rotate(m_position, delta_y * delta_time * y_weight, glm::vec3(1, 0, 0));
    }

    m_last_mouse_pos_x = xpos;
    m_last_mouse_pos_y = ypos;
}

void OrbitCamera::glfw_process_mouse_action(int button, int action, int mods, float delta_time)
{
    m_rotate_when_mouse_move = button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS;
}

void OrbitCamera::glfw_process_scroll(double xoffset, double yoffset, float delta_time)
{
    glm::vec3 direction = glm::normalize(m_position - m_target);
    m_position -= direction * float(yoffset) * 2000.0f * delta_time;
}

} // gui