#pragma once

#include "orbit_camera.h"
#include "shader.h"
#include "rasterized_points.h"

#include <cstdlib>
#include <cstdint>
#include <memory>
#include <string>

// Forward declarations.
class GLFWwindow;
namespace core { class ClosestPointQuery; }
namespace core { class MeshPointCloud; }
namespace core { class Scene; }

namespace gui
{

/**
 * @brief Standalone application.
 * 
 * GLFW is used to handle the window and the events.
 * Dear imgui is used to draw some UI elements.
 * 
 * This standalone application allow to load 3D models
 * and run the "closest point on mesh" algorithm on them.
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

    static MainWindow                         m_instance;

    GLFWwindow*                               m_glfw_window;
    int                                       m_screen_width;
    int                                       m_screen_height;
    bool                                      m_draw_wireframe;

    // Time data.
    float                                     m_time_since_startup;
    float                                     m_last_frame_time;
    float                                     m_frame_delta_time;

    // Framerate data.
    double                                    m_framerate;
    float                                     m_last_fps_capture_time;
    std::size_t                               m_fps_capture_frame_count;

    // 3D Scene.
    std::unique_ptr<core::Scene>              m_scene;        // scene actual data
    RasterizedPoints                          m_scene_points; // wrapper that render the scene

    // Shaders.
    std::unique_ptr<Shader>                   m_drawing_shader; // shader used to draw meshes
    std::unique_ptr<Shader>                   m_point_shader;   // shader used to draw query and algorithm points

    // Algorithm data.
    // Input data:
    // - a query point (3D position)
    // - a search radius (float)
    // - a mesh
    // Algorithm pre-generated objects: 
    // - a mesh point cloud
    // - a query object
    // Output data:
    // - the closest point on the mesh (3D position)
    // Other stored data:
    // - process time
    std::unique_ptr<core::MeshPointCloud>     m_mesh_point_cloud;     
    std::unique_ptr<core::ClosestPointQuery>  m_closest_point_query;  
    glm::vec3                                 m_query_point_pos;      
    float                                     m_query_point_max_serach_radius; 
    glm::vec3                                 m_closest_point_pos; 
    std::int64_t                              m_closest_point_query_time; // milliseconds
    bool                                      m_closest_point_found;

    // The application is able to move the query point.
    // I made this to quickly detect any incorrect behavior from the implementation.
    bool                                      m_animate_query_point;

    OrbitCamera                               m_camera;

    void process_glfw_window_inputs();

    void imgui_draw();
    void imgui_draw_main_menu_bar();

    void opengl_draw();

    void find_closest_point();
    void animate_query_point();

    // GLFW Window callbacks.
    static void glfw_framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void glfw_scroll_callback(GLFWwindow* window, double delta_x, double delta_y);
    static void glfw_key_callback(GLFWwindow* window, int key, int, int action, int mods);
    static void glfw_char_callback(GLFWwindow* window, unsigned int c);
    static void glfw_mouse_callback(GLFWwindow* window, double x, double y);
    static void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
};

} // namespace gui
