#pragma once

#include "orbit_camera.h"
#include "shader.h"

#include <memory>
#include <string>

// Forward declarations.
class GLFWwindow;
namespace core { class Scene; }

namespace gui
{ 

/**
 * @brief GUI Singleton.
 * Control the whole application UI using imgui.
 */
class MainWindow
{
  public:
    static MainWindow& get_instance();

    /**
     * @brief Open the window.
     */
    void init(const int window_width, const int window_height);

    void run();

    void release();

    void load_scene(const std::string& path);

  private:
    MainWindow();

    /**
     * @brief Copy is forbiden.
     */
    MainWindow(const MainWindow&) = delete;

    static MainWindow             m_instance;

    GLFWwindow*                   m_glfw_window;
    int                           m_screen_width;
    int                           m_screen_height;

    std::unique_ptr<core::Scene>  m_scene;
    std::unique_ptr<Shader>       m_drawing_shader;
    OrbitCamera                   m_camera;

    void process_glfw_window_inputs();

    void imgui_draw();
    void imgui_draw_main_menu_bar();

    void opengl_draw();

    // GLFW Window callbacks.
    static void glfw_framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void glfw_scroll_callback(GLFWwindow* window, double delta_x, double delta_y);
    static void glfw_key_callback(GLFWwindow* window, int key, int, int action, int mods);
    static void glfw_char_callback(GLFWwindow* window, unsigned int c);
    static void glfw_mouse_callback(GLFWwindow* window, double x, double y);
    static void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
};

} // namespace gui