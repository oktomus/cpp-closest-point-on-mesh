#include "orbit_camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <GLFW/glfw3.h>

#include <algorithm>

namespace gui
{

OrbitCamera::OrbitCamera()
{
    m_position = { 0, 0, 0 };
    m_target = { 0, 0, 0 };
    m_camera_pivot_rotation = { 0, 0 ,0 };
    m_distance_to_target = 20;
    m_fov = 45.0f;
    compute_position();
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
        float y_weight = std::abs(delta_y * 0.3f);

        m_camera_pivot_rotation.y += delta_x * delta_time * x_weight;
        m_camera_pivot_rotation.x += delta_y * delta_time * y_weight;
        compute_position();
    }
    else if (m_pan_when_mouse_move)
    {
        float delta_x = xpos - m_last_mouse_pos_x;
        float delta_y = ypos - m_last_mouse_pos_y;

        float x_weight = -std::abs(delta_x * 0.8f);

        m_target.y -= delta_y * delta_time * 20.0f;
        compute_position();
    }

    m_last_mouse_pos_x = xpos;
    m_last_mouse_pos_y = ypos;
}

void OrbitCamera::glfw_process_mouse_action(int button, int action, int mods, float delta_time)
{
    m_rotate_when_mouse_move = button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS;
    m_pan_when_mouse_move = !m_rotate_when_mouse_move && button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS;
}

void OrbitCamera::glfw_process_scroll(double xoffset, double yoffset, float delta_time)
{
    m_distance_to_target = std::max(1.0f, m_distance_to_target - float(yoffset) * 400.0f * delta_time);
    compute_position();
}

void OrbitCamera::compute_position()
{
    m_position = { m_distance_to_target, 0.0f, 0.0f };
    m_position = glm::rotate(m_position, m_camera_pivot_rotation.x, glm::vec3(0.0f, 0.0f, 1.0f));
    m_position = glm::rotate(m_position, m_camera_pivot_rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
}

} // gui