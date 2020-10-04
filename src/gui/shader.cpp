#include "shader.h"

#include <cassert>
#include <fstream>
#include <iostream>

namespace gui
{

namespace
{
    std::string read_shader_file(const std::string& path)
    {
        std::ifstream txtFile(path);

        if (!txtFile.is_open())
        {
            std::cerr << "ERROR::SHADER::FILE: Cannot read file " << path << "\n";
            return "";
        }

        std::string txtContent(
            (std::istreambuf_iterator<char>(txtFile)),
            (std::istreambuf_iterator<char>()));

        txtFile.close();

        return txtContent;
    }

    void check_shader_compile_sate(
        const GLuint&       shader_id, 
        const std::string&  type)
    {
        GLint code;

        bool shader_is_not_program = type != "PROGRAM";

        if (shader_is_not_program)
            glGetShaderiv(shader_id, GL_COMPILE_STATUS, &code);
        else
            glGetProgramiv(shader_id, GL_COMPILE_STATUS, &code);

        if (!code)
        {
            GLint logLength = 0;

            if (shader_is_not_program)
                glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &logLength);
            else
                glGetProgramiv(shader_id, GL_INFO_LOG_LENGTH, &logLength);

            std::string message;
            if (shader_is_not_program)
                message = "ERROR::SHADER_COMPILATION::" + type;
            else
                message = "ERROR::PROGRAM_LINK::" + type;

            if (logLength)
            {
                GLchar *log = new GLchar[logLength];
                GLsizei length = 0;
                if (shader_is_not_program)
                    glGetShaderInfoLog(shader_id, logLength, &length, log);
                else
                    glGetProgramInfoLog(shader_id, logLength, &length, log);
                message += "\n";
                message += log;
                delete[] log;
            }

            std::cerr << "Shader compilation failed." << "\n";
            std::cerr << message << "\n";
        }
    }

    void compile_shader(
        const std::string&  sourcePath,
        const GLuint&       shader_id,
        const std::string&  type)
    {
        assert(shader_id != 0);

        const std::string source = read_shader_file(sourcePath);
        const char *sourceChar = source.c_str();

        // Load the source
        glShaderSource(shader_id, 1, &sourceChar, nullptr);

        // Compile the source
        glCompileShader(shader_id);

        check_shader_compile_sate(shader_id, type);
    }
}

Shader::Shader(
    const std::string& vert_file_path, 
    const std::string& frag_file_path)
{
    GLuint vert_id = glCreateShader(GL_VERTEX_SHADER);
    GLuint frag_id = glCreateShader(GL_FRAGMENT_SHADER);

    compile_shader(vert_file_path, vert_id, "VERTEX");
    compile_shader(frag_file_path, frag_id, "FRAGMENT");

    m_shader_program_id = glCreateProgram();
    glAttachShader(m_shader_program_id, vert_id);
    glAttachShader(m_shader_program_id, frag_id);

    glLinkProgram(m_shader_program_id);
    check_shader_compile_sate(m_shader_program_id, "PROGRAM");

    glDeleteShader(vert_id);
    glDeleteShader(frag_id);
}

void Shader::use() const
{
   glUseProgram(m_shader_program_id);
}

} // gui