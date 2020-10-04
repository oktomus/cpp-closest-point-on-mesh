#include "mainwindow.h"

// gui includes.
#include "file_system.h"

// core includes.
#include "core/scene.h"
#include "core/scene_loader.h"

// imgui, gl3w and glfw includes.
#include <imgui.h>
#include "imgui_impl_glfw_gl3.h"
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

// Standard includes.
#include <cassert>
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
    glfwSetFramebufferSizeCallback(m_glfw_window,   this->glfw_framebuffer_size_callback);
    glfwSetCursorPosCallback(m_glfw_window,         this->glfw_mouse_callback);
    glfwSetScrollCallback(m_glfw_window,            this->glfw_scroll_callback);
    glfwSetMouseButtonCallback(m_glfw_window,       this->glfw_mouse_button_callback);
    glfwSetKeyCallback(m_glfw_window,               this->glfw_key_callback);
    glfwSetCharCallback(m_glfw_window,              this->glfw_char_callback);

    std::cout << "Loading OpenGL...\n";

    if (gl3wInit()) 
    {
        std::cerr << "Failed to initialize OpenGL.\n";
        exit(4);
    }

    printf(
        "Loaded OpenGL %s, GLSL %s\n", 
        glGetString(GL_VERSION),
        glGetString(GL_SHADING_LANGUAGE_VERSION));

    std::cout << "Compiling shaders...\n";
    m_drawing_shader = std::make_unique<Shader>("resources/shaders/solid.vert", "resources/shaders/solid.frag");

    std::cout << "Loading imgui...\n";
    ImGui_ImplGlfwGL3_Init(m_glfw_window, false);

    std::cout << "GUI ready to draw.\n";
}

void MainWindow::run()
{
    while (!glfwWindowShouldClose(m_glfw_window))
    {
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
    m_scene.reset(core::load_scene_from_file(path));
}

// Constructor.
MainWindow::MainWindow() {}

// Singleton instance.
MainWindow MainWindow::m_instance;

void MainWindow::process_glfw_window_inputs()
{
    if (ImGui::IsAnyWindowHovered()) return;
}

void MainWindow::imgui_draw()
{
    static bool debug_window_ = true;
    static bool scene_window = true;

    // Top bar
    if (ImGui::BeginMainMenuBar())
    {
        imgui_draw_main_menu_bar();
        ImGui::EndMainMenuBar();
    }

    // Debug win
    if (debug_window_)
    {
        ImGui::Begin("Debug", &debug_window_);

        //ImGui::Checkbox("Disable viewport events", &disableViewportEvents_);
        //ImGui::Text("%f ms/frame, ~%d FPS", frameRate_,
                    //(long)(1000 / frameRate_));
        //ImGui::ColorEdit4("Clear color", glm::value_ptr(clearColor_));
        ImGui::End();
    }

    if (scene_window)
    {
        ImGui::Begin("Scene", &scene_window);

        //ImGui::Checkbox("Wireframe mode", &drawLine_);
        //ImGui::Checkbox("Display lights in the viewport", &drawLights_);

        if (ImGui::CollapsingHeader("Skybox"))
        {
            //ImGui::Checkbox("Show sky", &skybox_->draw_);
        }

        if (ImGui::CollapsingHeader("Terrain"))
        {
            //ImGui::Checkbox("Show terrain", &ground_->draw_);

            //if (ImGui::Button("Randomize Terrain"))
                //ground_->randomize();
        }

        if (ImGui::CollapsingHeader("Camera"))
        {
            //ImGui::Text("Camera Pos (%f, %f, %f)", cam_->pos.x, cam_->pos.y,
                        //cam_->pos.z);
            if (ImGui::Button("Reset Camera"))
            {
                //cam_->reset();
            }
            if (ImGui::Button("Switch to FPS"))
            {
                //cam_ = std::make_unique<FPSCamera>();
            }
            if (ImGui::Button("Switch to Orbit"))
            {
                //cam_ = std::make_unique<OrbitCamera>();
            }
        }

        if (ImGui::CollapsingHeader("Objects"))
        {
        }

        if (ImGui::CollapsingHeader("Lighting"))
        {
        }

        ImGui::End();
    }
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

    if (ImGui::BeginMenu("Windows"))
    {
        //ImGui::MenuItem("Debug", "", &debug_window_);
        //ImGui::MenuItem("Objects", "", &scene_window);
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

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_drawing_shader->use();
    m_drawing_shader->set_mat4("view", m_camera.view());
    //m_drawing_shader->set_vec3("viewPos", m_camera.get_position());
    m_drawing_shader->set_mat4("projection", m_camera.projection(m_screen_width, m_screen_height));

    if (m_scene)
        m_scene->render();
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

    gui.m_camera.glfw_process_scroll(gui.m_glfw_window, delta_x, delta_y);
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

    gui.m_camera.glfw_process_mouse_move(gui.m_glfw_window, x, y);
}

void MainWindow::glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    MainWindow &gui = MainWindow::get_instance();

    ImGui_ImplGlfwGL3_MouseButtonCallback(gui.m_glfw_window, button, action, mods);
    if (ImGui::IsAnyWindowHovered())
        return;

    gui.m_camera.glfw_process_mouse_action(gui.m_glfw_window, button, action, mods);
}

} // namespace gui