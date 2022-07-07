#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "shader.hpp"
#include <iostream>

#include <cstdio>
#include <cmath>
#include <cassert>

int W = 1920;
int H = 1080;

using namespace std;

static void __key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwDestroyWindow(window);
}

int main()
{
    assert(glfwInit());
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWmonitor *   monitor     =   glfwGetPrimaryMonitor();
    GLFWwindow  *   window      =   glfwCreateWindow(W, H, "Ray Casting", monitor, NULL);
    const GLFWvidmode * mode    =   glfwGetVideoMode(monitor);
    W = mode->width;
    H = mode->height;

    if (!window) {glfwTerminate(); return -1;}
    glfwSetKeyCallback(window, __key_callback);
    glfwMakeContextCurrent(window);
    gladLoadGL();
    glfwSwapInterval(1);

    const timespec  timer       =   {0, 99999};
    GLuint          program     =   create_shader_program("shader.vert.cpp", "shader.frag.cpp");
    GLfloat     *   vertexes    =   new float[H*W*2];
    double          mouseDiff[2];
    double          mouseAbs[2] ;
    float           position[3] =   {-5.0, 0.0, 0.0};

    int state_key_W;
    int state_key_S;
    int state_key_A;
    int state_key_D;
    int state_key_X;
    int state_key_space;

    for(int i = 0; i < H; i++)
        for(int j = 0; j < W; j++)
            { vertexes[2*i*W+j*2] = -1 + 2*j/(double)W; vertexes[2*i*W+j*2+1] = -1 + 2*i/(double)H; }

    glViewport(0,0, W,H);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(1,1,1,0);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwGetCursorPos(window, &mouseDiff[0], &mouseDiff[1]);
        mouseDiff[0] = (mouseDiff[0] - W/2)/(double)W*M_PI/1.5;
        mouseDiff[1] = -(mouseDiff[1] - H/2)/(double)H*M_PI/1.5;
        mouseAbs[0] += mouseDiff[0];
        mouseAbs[1] += mouseDiff[1];
        glfwSetCursorPos(window, W/2.0, H/2.0);

        static float u_time_val = 0;

        state_key_W = glfwGetKey(window, GLFW_KEY_W);
        state_key_S = glfwGetKey(window, GLFW_KEY_S);
        state_key_A = glfwGetKey(window, GLFW_KEY_A);
        state_key_D = glfwGetKey(window, GLFW_KEY_D);
        state_key_X = glfwGetKey(window, GLFW_KEY_X);
        state_key_space = glfwGetKey(window, GLFW_KEY_SPACE);

        if(state_key_W == GLFW_PRESS)
            { position[0] += 0.02; }
        if(state_key_S == GLFW_PRESS)
            { position[0] -= 0.02; }
        if(state_key_A == GLFW_PRESS)
            { position[2] -= 0.02; }
        if(state_key_D == GLFW_PRESS)
            { position[2] += 0.02; }
        if(state_key_X == GLFW_PRESS)
            { position[1] -= 0.02; }
        if(state_key_space == GLFW_PRESS)
            { position[1] += 0.02; }

        int u_resolution = glGetUniformLocation(program, "u_resolution");
        int u_time = glGetUniformLocation(program, "u_time");
        int u_mouse = glGetUniformLocation(program, "u_mouse");
        int u_position = glGetUniformLocation(program, "u_position");

        glUniform2f(u_resolution, (float)W, (float)H);
        glUniform1f(u_time, u_time_val+=0.01);
        glUniform2f(u_mouse, (float)mouseAbs[0], (float)mouseAbs[1]);
        glUniform3f(u_position, position[0], position[1], position[2]);
        glUseProgram(program);

        glVertexPointer(2, GL_FLOAT, 0, vertexes);
        glEnableClientState(GL_VERTEX_ARRAY);
        glDrawArrays(GL_POINTS, 0, H*W);
        glDisableClientState(GL_VERTEX_ARRAY);

        glfwSwapBuffers(window);
        glfwPollEvents();
        nanosleep(&timer, NULL);
    }

    glfwTerminate();
    return 0;
}
