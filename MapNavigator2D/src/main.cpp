#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../include/util.hpp"
#include "../include/map.hpp"
#include "../include/overlay.hpp"

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primary);

    GLFWwindow* window = glfwCreateWindow(
        mode->width,
        mode->height,
        "MapNavigator2D",
        NULL,
        NULL
    );
    if (!window) return endProgram("Window initialization failed.");

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
        return endProgram("GLEW initialization failed.");

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
    Overlay overlay("textures/pin.png", "textures/walking_icon.png");
    map.viewFraction = 0.5f;
    overlay.setWalkingMode(true);

    // initially centered
    map.offsetX_norm = 0.5f - map.viewFraction / 2.0f;
    map.offsetY_norm = 0.5f - map.viewFraction / 2.0f;


    // MAIN LOOP
    double lastTime = glfwGetTime();
    bool rWasPressed = false;

    int fbW, fbH;
    glfwGetFramebufferSize(window, &fbW, &fbH);
    glViewport(0, 0, fbW, fbH);
    
    while (!glfwWindowShouldClose(window))
    {
        double now = glfwGetTime();
        float deltaTime = (float)(now - lastTime);
        lastTime = now;

        if (overlay.isWalkingMode())
        {
            float dxPix = 0.0f, dyPix = 0.0f;
            float speed = map.moveSpeedPixels;

            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                dyPix -= speed * deltaTime;

            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                dyPix += speed * deltaTime;

            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                dxPix -= speed * deltaTime;

            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                dxPix += speed * deltaTime;

            map.walkedDistancePixels += map.applyMovementAndMeasure(dxPix, dyPix, fbW, fbH);
        }


        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        {
            if (!rWasPressed)
            {
                overlay.setWalkingMode(!overlay.isWalkingMode());
                if (overlay.isWalkingMode()) map.viewFraction = 0.5f;
                else map.viewFraction = 1.0f;
            }
            rWasPressed = true;
        }
        else rWasPressed = false;

        glClear(GL_COLOR_BUFFER_BIT);

        
        glUniform1i(glGetUniformLocation(shaderProgram, "uIgnoreTransform"), false);
        map.bindShaderTransform(shaderProgram, fbW, fbH);

        glUseProgram(shaderProgram);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, map.getTextureID());
        glBindVertexArray(map.getVAO());
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        overlay.drawPin(shaderProgram, fbW, fbH);
        overlay.drawWalkIcon(shaderProgram, fbW, fbH);
        overlay.loadFont("fonts/arial.ttf", 24);

        if (overlay.isWalkingMode()) {
            overlay.drawFilledRect(120, fbH - 150, 300, 40, 0, 0, 0, fbW, fbH);
            std::string dist = "DISTANCE: " + std::to_string((int)map.walkedDistancePixels) + " px";
            overlay.drawText(dist.c_str(), 150, 140, 1.0f, 1, 1, 1, fbW, fbH);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

