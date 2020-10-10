#include "mainwindow.h"

// gui includes.
#include "file_system.h"

// core includes.
#include "core/closest_point_query.h"
#include "core/scene.h"
#include "core/scene_loader.h"

// imgui, gl3w and glfw includes.
#include <imgui.h>
#include "imgui_impl_glfw_gl3.h"
#include "imgui_internal.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

// Standard includes.
#include <cassert>
#include <chrono>
#include <inttypes.h>
#include <iostream>
#include <string>

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
    m_glfw_window = glfwCreateWindow(window_width, window_height, "app.gui", NULL, NULL);

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

        process_glfw_window_inputs();
        ImGui_ImplGlfwGL3_NewFrame();

        // Run query.
        if (m_closest_point_query)
            find_closest_point();

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
    m_scene.reset(core::load_scene_from_file(path));
    core::ClosestPointQuery* query = new core::ClosestPointQuery(m_scene->get_mesh());
    m_closest_point_query.reset(query);
}

// Constructor.
MainWindow::MainWindow()
  : m_query_point_max_serach_radius(10.0f)
  , m_query_point_pos(-1.2f, 1.6f, 2.8f)
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

        if (first_display) ImGui::SetNextTreeNodeOpen(true);
        if (ImGui::TreeNode("Closest point query settings"))
        {
            ImGui::Text("Position");
            ImGui::DragFloat3("", glm::value_ptr(m_query_point_pos), 0.1f);
            ImGui::Text("Maximum search radius");
            ImGui::DragFloat("", &m_query_point_max_serach_radius, 0.1f, 0.0f);
            m_query_point_max_serach_radius = std::max(m_query_point_max_serach_radius, 0.0f);
            ImGui::TreePop();
        }

        if (first_display) ImGui::SetNextTreeNodeOpen(true);
        if (ImGui::TreeNode("Closest point result"))
        {
            ImGui::Text("Position");
            glm::vec3 readonly_pos = m_closest_point_pos;
            ImGui::DragFloat3("", glm::value_ptr(readonly_pos));
            ImGui::Text("Time %" PRId64 "ms", m_closest_point_query_time);
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

    // Run the query.
    bool found = run && m_closest_point_query->get_closest_point(
        m_query_point_pos,
        m_query_point_max_serach_radius,
        m_closest_point_pos);

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
    if (found)
        points.push_back({
            m_closest_point_pos,
            { 0.1, 1.0, 0.1, 1.0 }
        });

    m_scene_points.set_points(points);
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
