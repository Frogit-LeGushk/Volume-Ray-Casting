#include <glad/glad.h>

#include <stdio.h>
#include "shader.hpp"

static char *load_shader_file(const char *filename)
{
    static char program_text[(1 << 12)];
    FILE *fp = fopen(filename, "rb");
    if(!fp) printf("Error load shader file");
    size_t size_read = fread(program_text, sizeof(char), (1 << 12) - 2, fp);
    program_text[size_read] = 0;
    return program_text;
}
static void check_compile_status(GLuint shader)
{
    GLint status_ok;
    static char status_log[(1 << 12)] = "";
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status_ok);
    if(!status_ok)
    {
        glGetShaderInfoLog(shader, (1 << 12) - 2, NULL, status_log);
        printf("COMPILE STATUS LOG: %s\n", status_log);
    }
}
static void check_link_status(GLuint shader)
{
    GLint status_ok;
    static char status_log[(1 << 12)];
    glGetProgramiv(shader, GL_LINK_STATUS, &status_ok);
    if(!status_ok)
    {
        glGetProgramInfoLog(shader, (1 << 12) - 2, NULL, status_log);
        printf("LINK STATUS LOG: %s\n", status_log);
    }
}
static GLuint load_shader(const char *filename, GLuint type)
{
    char *text = load_shader_file(filename);
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &text, NULL);

    glCompileShader(shader);
    check_compile_status(shader);
    return shader;
}
GLuint create_shader_program(const char *vertex_file_name, const char *fragment_file_name)
{
    GLuint f_shader;
    GLuint v_shader;

    if(fragment_file_name)
        f_shader = load_shader(fragment_file_name, GL_FRAGMENT_SHADER);
    if(vertex_file_name)
        v_shader = load_shader(vertex_file_name, GL_VERTEX_SHADER);

    GLuint program = glCreateProgram();

    if(fragment_file_name)
        glAttachShader(program, f_shader);
    if(vertex_file_name)
        glAttachShader(program, v_shader);

    glLinkProgram(program);

    if(fragment_file_name)
        check_link_status(f_shader);
    if(vertex_file_name)
        check_link_status(v_shader);

    return program;
}
