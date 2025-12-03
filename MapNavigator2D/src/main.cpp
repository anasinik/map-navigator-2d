#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../include/util.hpp"
#include "../include/map.hpp"

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primary);
    
    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "MapNavigator2D", NULL, NULL);
    if (window == NULL) return endProgram("Window initialization failed.");

    glfwMakeContextCurrent(window);


    if (glewInit() != GLEW_OK) return endProgram("GLEW initialization failed");

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.757f, 0.761f, 0.753f, 1.0f);

    unsigned int shaderProgram = createShader(
        "shaders/vertex_shader.vert",
        "shaders/fragment_shader.frag"
    );

    glUseProgram(shaderProgram);

    glUniform1i(glGetUniformLocation(shaderProgram, "uTexture"), 0);

    Map map("textures/novi-sad-map.jpg");
    int fbW, fbH;
    glfwGetFramebufferSize(window, &fbW, &fbH);
    map.bindShaderTransform(shaderProgram, fbW, fbH);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        map.bindShaderTransform(shaderProgram, mode->width, mode->height);

        glUseProgram(shaderProgram);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, map.getTextureID());
        glBindVertexArray(map.getVAO());
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}