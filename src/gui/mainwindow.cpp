#include "mainwindow.h"

// gui includes.
#include "file_system.h"

// core includes.
#include "core/closest_point_query.h"
#include "core/mesh_point_cloud.h"
#include "core/scene.h"
#include "core/scene_loader.h"

// imgui, gl3w and glfw includes.
#include <imgui.h>
#include "imgui_impl_glfw_gl3.h"
#include "imgui_internal.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

// Standard includes.
#include <cassert>
#include <chrono>
#include <inttypes.h>
#include <iostream>
#include <string>

// FIXME: This is not the cleanest code from this project.
// Here are some refactoring ideas:
// - Split IMGUI and OpenGL drawing in 2 separate classes.
// - Store algorithm related data in a separate class.

namespace gui
{

// Singleton access.
MainWindow& MainWindow::get_instance()
{
    return MainWindow::m_instance;
}

namespace
{
    /**
     * @brief Handle glfw errors
     */
    void glfw_error_callback(int error, const char* description)
    {
        std::cerr << "GLFW Error " << error << ": " << description << "\n";
    }
}

void MainWindow::init(const int window_width, const int window_height)
{
    static bool opened = false;
    assert(!opened);
    opened = true;

    m_screen_width = window_width;
    m_screen_height = window_height;

    std::cout << "Initializing window...\n";
    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit())
    {
      std::cerr << "Unable to initialize GLFW.\n";
      exit(1);
    }

    // Use OpenGL 3.3.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* monitor =  glfwGetPrimaryMonitor();
    const GLFWvidmode* video_mode = glfwGetVideoMode(monitor);

    m_glfw_window = glfwCreateWindow(video_mode->width, video_mode->height, "app.gui", NULL, NULL);

    if (!m_glfw_window)
    {
      std::cerr << "Unable to create a window.\n";
      exit(2);
    }

    glfwMakeContextCurrent(m_glfw_window);
    glfwSwapInterval(0);
    glfwSetFramebufferSizeCallback(m_glfw_window,   this->glfw_framebuffer_size_callback);
    glfwSetCursorPosCallback(m_glfw_window,         this->glfw_mouse_callback);
    glfwSetScrollCallback(m_glfw_window,            this->glfw_scroll_callback);
    glfwSetMouseButtonCallback(m_glfw_window,       this->glfw_mouse_button_callback);
    glfwSetKeyCallback(m_glfw_window,               this->glfw_key_callback);
    glfwSetCharCallback(m_glfw_window,              this->glfw_char_callback);

    std::cout << "Loading OpenGL...\n";

    if (!gladLoadGL())
    {
        std::cerr << "Failed to initialize OpenGL.\n";
        exit(4);
    }

    printf(
        "Loaded OpenGL %s, GLSL %s\n",
        glGetString(GL_VERSION),
        glGetString(GL_SHADING_LANGUAGE_VERSION));

    std::cout << "Compiling shaders...\n";
    m_drawing_shader = std::make_unique<Shader>("resources/shaders/facing_ratio.vert", "resources/shaders/facing_ratio.frag");
    m_point_shader = std::make_unique<Shader>("resources/shaders/point.vert", "resources/shaders/point.frag");
    // m_drawing_shader = std::make_unique<Shader>("resources/shaders/solid.vert", "resources/shaders/solid.frag");

    std::cout << "Loading imgui...\n";
    ImGui_ImplGlfwGL3_Init(m_glfw_window, false);

    std::cout << "GUI ready to draw.\n";
}

void MainWindow::run()
{
    while (!glfwWindowShouldClose(m_glfw_window))
    {
        m_fps_capture_frame_count += 1;
        m_time_since_startup = glfwGetTime();
        m_frame_delta_time = m_time_since_startup - m_last_frame_time;
        m_last_frame_time = m_time_since_startup;

        // Compute FPS every one second.
        if (m_time_since_startup - m_last_fps_capture_time >= 1.0)
        {
            m_framerate = 1000.0 / double(m_fps_capture_frame_count);
            m_fps_capture_frame_count = 0;
            m_last_fps_capture_time += 1.0;
        }

        // Animation query point.
        if (m_animate_query_point)
            animate_query_point();

        // Run query.
        if (m_closest_point_query)
            find_closest_point();

        process_glfw_window_inputs();
        ImGui_ImplGlfwGL3_NewFrame();

        imgui_draw();
        opengl_draw();

        glFlush();
        ImGui::Render();
        glfwPollEvents();
        glfwSwapBuffers(m_glfw_window);
    }
}

void MainWindow::release()
{
    ImGui_ImplGlfwGL3_Shutdown();
    glfwTerminate();
}

void MainWindow::load_scene(const std::string& path)
{
    // Stop the running animation.
    m_animate_query_point = false;

    // Load the scene.
    m_scene.reset(core::load_scene_from_file(path));

    if (m_scene->get_mesh_count() == 0)
    {
        std::cerr << "No mesh in the scene.\n";
        m_mesh_point_cloud.reset(nullptr);
        m_closest_point_query.reset(nullptr);
        return;
    }

    // Build a point cloud of the mesh.
    m_mesh_point_cloud.reset(new core::MeshPointCloud(m_scene->get_mesh(0)));

    // Prepare closest point queries.
    m_closest_point_query.reset(new core::ClosestPointQuery(*m_mesh_point_cloud));
}

// Constructor.
MainWindow::MainWindow()
  : m_query_point_max_serach_radius(10.0f)
  , m_query_point_pos(1.0f, 1.0f, 1.0f)
  , m_animate_query_point(false)
  , m_query_count(1)
{}

// Singleton instance.
MainWindow MainWindow::m_instance;

void MainWindow::process_glfw_window_inputs()
{
    if (ImGui::IsAnyWindowHovered()) return;
}

void MainWindow::imgui_draw()
{
    static bool show_window = true;
    static bool first_display = true;

    // Top bar
    if (ImGui::BeginMainMenuBar())
    {
        imgui_draw_main_menu_bar();
        ImGui::EndMainMenuBar();
    }

    // Debug win
    if (show_window)
    {
        ImGui::Begin("Controls", &show_window);

        ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "In white: the query point");
        ImGui::TextColored(ImVec4(0.1f, 1.0f, 0.1f, 1.0f), "In green: result point (closest point on the mesh)");

        ImGui::Spacing();

        if (first_display) ImGui::SetNextTreeNodeOpen(true);
        if (ImGui::TreeNode("Closest point query settings"))
        {
            ImGui::DragFloat3("Position", glm::value_ptr(m_query_point_pos), 0.05f);

            float search_radius = m_query_point_max_serach_radius;
            ImGui::DragFloat("Search radius", &search_radius, 0.5f, 0.0f, 100.0f);
            if (search_radius != m_query_point_max_serach_radius)
                m_query_point_max_serach_radius = search_radius;

            ImGui::DragInt("Query count", &m_query_count, 1, 1, 1000);

            if (!m_animate_query_point && ImGui::Button("Animate query point"))
            {
                m_animate_query_point = true;
            }
            else if (m_animate_query_point && ImGui::Button("Stop animation"))
            {
                m_animate_query_point = false;
            }

            ImGui::TreePop();
        }

        if (first_display) ImGui::SetNextTreeNodeOpen(true);
        if (ImGui::TreeNode("Closest point result"))
        {
            glm::vec3 readonly_pos = m_closest_point_pos;
            ImGui::DragFloat3("Position", glm::value_ptr(readonly_pos));
            ImGui::Text("Last query time %" PRId64 "ms", m_closest_point_query_time);
            ImGui::TreePop();
        }

        if (first_display) ImGui::SetNextTreeNodeOpen(true);
        if (ImGui::TreeNode("Camera"))
        {
            float fov = m_camera.get_fov();
            ImGui::DragFloat("FOV", &fov, 0.5f, 1.0f, 90.0f);
            if (fov != m_camera.get_fov())
                m_camera.set_fov(fov);

            float distance = m_camera.get_distance_to_target();
            ImGui::DragFloat("Distance", &distance, 0.5f, 0.0f, 100.0f);
            if (fov != m_camera.get_distance_to_target())
                m_camera.set_distance_to_target(distance);

            ImGui::TreePop();
        }

        if (first_display) ImGui::SetNextTreeNodeOpen(true);
        if (ImGui::TreeNode("Debug"))
        {
            ImGui::Text(
                "%f ms/frame, ~%d FPS",
                m_framerate,
                (long)(1000.0 / m_framerate));

            ImGui::Text("Mouse Position: (%.1f,%.1f)", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
            ImGui::TreePop();
        }

        ImGui::End();
    }

    first_display = false;
}

void MainWindow::imgui_draw_main_menu_bar()
{
    if (ImGui::BeginMenu("App"))
    {
        static bool should_quit = false;
        ImGui::MenuItem("Quit", "Escape", &should_quit);
        if (should_quit)
            glfwSetWindowShouldClose(m_glfw_window, true);
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Mesh"))
    {
        if (ImGui::MenuItem("Load file", "", false))
        {
            const std::vector<std::string> filter = { "OBJ | *.obj", "All files | *"};
            const std::string file_path = linux_open_file("Choose a mesh", filter);
            if (!file_path.empty())
                load_scene(file_path);
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Viewport"))
    {
        ImGui::MenuItem("Draw wireframe", "", &m_draw_wireframe);
        ImGui::EndMenu();
    }
}

void MainWindow::opengl_draw()
{
    glClearColor(0.12, 0.12, 0.24, 1.0);
    glClearDepth(1.0);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    glEnable(GL_POINT_SIZE);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (m_draw_wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const glm::mat4 view = m_camera.view();
    const glm::mat4 projection = m_camera.projection(m_screen_width, m_screen_height);

    // Draw the scene.
    m_drawing_shader->use();
    m_drawing_shader->set_mat4("view", view);
    m_drawing_shader->set_mat4("projection", projection);

    if (m_scene)
        m_scene->render();

    // Draw the points to showcase the algorithm.
    glDisable(GL_DEPTH_TEST);
    m_point_shader->use();
    m_point_shader->set_mat4("view", view);
    m_point_shader->set_mat4("projection", projection);
    m_point_shader->set_float("size", 8.0f);
    m_scene_points.draw();
}

void MainWindow::find_closest_point()
{
    // Invalid search radius, don't run.
    bool run = m_query_point_max_serach_radius > 0.0f;

    // Start a timer to know how long it takes.
    auto timer_start = std::chrono::high_resolution_clock::now();

    // Run the query (multiple times if you want to see it's speed).
    for (int i = 0; i < m_query_count; ++i)
    {
        m_closest_point_found = run && m_closest_point_query->get_closest_point(
            m_query_point_pos,
            m_query_point_max_serach_radius,
            m_closest_point_pos);
    }

    auto timer_stop = std::chrono::high_resolution_clock::now();
    m_closest_point_query_time = std::chrono::duration_cast<std::chrono::milliseconds>(timer_stop - timer_start).count();

    // Push the points in a buffer ready to be rendered.
    std::vector<RasterizedPoints::Point> points;
    // Add the query point (in grey).
    points.push_back({
        m_query_point_pos,
        { 0.8, 0.8, 0.8, 1.0 }
    });
    // Add the closest result point (in green).
    if (m_closest_point_found)
        points.push_back({
            m_closest_point_pos,
            { 0.1, 1.0, 0.1, 1.0 }
        });

    m_scene_points.set_points(points);
}

void MainWindow::animate_query_point()
{
    // Rotate the query point arround the model.
    const float rotation =
        m_frame_delta_time
        + std::sin(m_time_since_startup * 0.5f) * m_frame_delta_time;
    m_query_point_pos -= m_closest_point_pos;
    m_query_point_pos =
        glm::rotate(
            m_query_point_pos,
            rotation,
            glm::vec3(1.0f, 1.0f, 1.0f));
    m_query_point_pos += m_closest_point_pos;
}

// GLFW Window callbacks.
void MainWindow::glfw_framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    MainWindow& gui = MainWindow::get_instance();
    gui.m_screen_width = width;
    gui.m_screen_height = height;
    glViewport(0, 0, width, height);
}

void MainWindow::glfw_scroll_callback(GLFWwindow* window, double delta_x, double delta_y)
{
    MainWindow& gui = MainWindow::get_instance();
    ImGui_ImplGlfwGL3_ScrollCallback(gui.m_glfw_window, delta_x, delta_y);

    if (ImGui::IsAnyWindowHovered())
        return;

    gui.m_camera.glfw_process_scroll(delta_x, delta_y, gui.m_frame_delta_time);
}

void MainWindow::glfw_key_callback(GLFWwindow* window, int key, int, int action, int mods)
{
    ImGui_ImplGlfwGL3_KeyCallback(0, key, 0, action, mods);
    if (ImGui::IsAnyWindowHovered())
        return;

    // keyboard shortcuts
    MainWindow& gui = MainWindow::get_instance();

    if (key == GLFW_KEY_ESCAPE)
        glfwSetWindowShouldClose(gui.m_glfw_window, true);
}

void MainWindow::glfw_char_callback(GLFWwindow* window, unsigned int c)
{
    ImGui_ImplGlfwGL3_CharCallback(0, c);
    if (ImGui::IsAnyWindowHovered())
        return;
}

void MainWindow::glfw_mouse_callback(GLFWwindow* window, double x, double y)
{
    // When true, the mouse interaction is disabled on the GLFW window.
    // We do this to not interact with GLFW when we use imgui.
    static bool ignore_glfw_mouse_events = false;

    MainWindow &gui = MainWindow::get_instance();

    bool is_mouse_on_imgui = ImGui::IsAnyWindowHovered();

    if (is_mouse_on_imgui && ignore_glfw_mouse_events)
    {
        glfwSetInputMode(gui.m_glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        ignore_glfw_mouse_events = false;
    }
    else if (!is_mouse_on_imgui && !ignore_glfw_mouse_events)
    {
        glfwSetInputMode(gui.m_glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        ignore_glfw_mouse_events = true;
    }

    gui.m_camera.glfw_process_mouse_move(x, y, gui.m_frame_delta_time);
}

void MainWindow::glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    MainWindow &gui = MainWindow::get_instance();

    ImGui_ImplGlfwGL3_MouseButtonCallback(gui.m_glfw_window, button, action, mods);
    if (ImGui::IsAnyWindowHovered())
        return;

    gui.m_camera.glfw_process_mouse_action(button, action, mods, gui.m_frame_delta_time);
}

} // namespace gui
