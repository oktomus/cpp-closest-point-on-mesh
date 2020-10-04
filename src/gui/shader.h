#pragma once

#include <glm/glm.hpp>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <string>

namespace gui
{

class Shader
{
  public:
    /**
     * @brief Create and compile a shader
     */
    Shader(
        const std::string& vert_file_path,
        const std::string& frag_file_path);

    /**
     * @brief Use the shader for rendering.
     */
    void use() const;

    void set_bool(const std::string &name, bool value) const
    {
        glUniform1i(glGetUniformLocation(m_shader_program_id, name.c_str()), (int)value);
    }

    void set_int(const std::string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(m_shader_program_id, name.c_str()), value);
    }

    void set_float(const std::string &name, float value) const
    {
        glUniform1f(glGetUniformLocation(m_shader_program_id, name.c_str()), value);
    }

    void set_vec2(const std::string &name, const glm::vec2 &value) const
    {
        glUniform2fv(glGetUniformLocation(m_shader_program_id, name.c_str()), 1, &value[0]);
    }
    void set_vec2(const std::string &name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(m_shader_program_id, name.c_str()), x, y);
    }

    void set_vec3(const std::string &name, const glm::vec3 &value) const
    {
        glUniform3fv(glGetUniformLocation(m_shader_program_id, name.c_str()), 1, &value[0]);
    }

    void set_vec3(const std::string &name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(m_shader_program_id, name.c_str()), x, y, z);
    }

    void set_vec4(const std::string &name, const glm::vec4 &value) const
    {
        glUniform4fv(glGetUniformLocation(m_shader_program_id, name.c_str()), 1, &value[0]);
    }

    void set_vec4(const std::string &name, float x, float y, float z, float w)
    {
        glUniform4f(glGetUniformLocation(m_shader_program_id, name.c_str()), x, y, z, w);
    }

    void set_mat2(const std::string &name, const glm::mat2 &mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(m_shader_program_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void set_mat3(const std::string &name, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(m_shader_program_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void set_mat4(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(m_shader_program_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

  private:
    /**
     * @brief Id of the program
     */
    GLuint m_shader_program_id;
};

} // gui