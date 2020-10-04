#pragma once

// Forward declarations.
class GLFWwindow;

namespace gui
{ 

/**
 * @brief GUI Singleton.
 * Control the whole application UI using imgui.
 */
class Gui
{
  public:
    static Gui& get_instance();

    /**
     * @brief Open the window.
     */
    void init(const int window_width, const int window_height);

    void run();

    void release();

  private:
    Gui();

    /**
     * @brief Copy is forbiden.
     */
    Gui(const Gui&) = delete;

    static Gui m_instance;

    GLFWwindow* m_glfw_window;

    void process_glfw_window_inputs();

    void imgui_draw();
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