#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../include/util.hpp"
#include "../include/map.hpp"
#include "../include/overlay.hpp"

int main()
{
    if (!glfwInit()) return endProgram("GLFW init failed");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primary);

    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "MapNavigator2D", NULL, NULL);
    if (!window) return endProgram("Window creation failed");

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
        return endProgram("GLEW init failed");

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.757f, 0.761f, 0.753f, 1.0f);

    unsigned int shaderProgram = createShader("shaders/vertex_shader.vert", "shaders/fragment_shader.frag");
    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "uTexture"), 0);

    Map map("textures/novi-sad-map.jpg");
    Overlay overlay("textures/pin.png", "textures/walking_icon.png");
    overlay.loadFont("fonts/arial.ttf", 24);

    map.viewFraction = 0.5f;
    overlay.setWalkingMode(true);

    // initially centered
    map.offsetX_norm = 0.5f - map.viewFraction / 2.0f;
    map.offsetY_norm = 0.5f - map.viewFraction / 2.0f;

    double lastTime = glfwGetTime();
    bool rWasPressed = false;
    bool clickHandled = false;
    double mouseX, mouseY;
    bool mapClickHandled = false;

    int fbW, fbH;
    glfwGetFramebufferSize(window, &fbW, &fbH);
    glViewport(0, 0, fbW, fbH);

    while (!glfwWindowShouldClose(window))
    {
        // EXIT ON ESC
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) 
            glfwSetWindowShouldClose(window, true);

        double initFrameTime = glfwGetTime();
        float deltaTime = (float)(initFrameTime - lastTime);
        lastTime = initFrameTime;

        // WALKING MODE
        if (overlay.isWalkingMode())
        {
            float dxPix = 0.0f, dyPix = 0.0f;
            float speed = map.moveSpeedPixels;

            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) dyPix -= speed * deltaTime;
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) dyPix += speed * deltaTime;
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) dxPix -= speed * deltaTime;
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) dxPix += speed * deltaTime;

            map.walkedDistancePixels += map.applyMovementAndMeasure(dxPix, dyPix, fbW, fbH);
        }

        // MOUSE
        glfwGetCursorPos(window, &mouseX, &mouseY);
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            if (!clickHandled &&
                mouseX >= overlay.walkIconX_px &&
                mouseX <= overlay.walkIconX_px + overlay.walkIconWidth_px &&
                mouseY >= overlay.walkIconY_px &&
                mouseY <= overlay.walkIconY_px + overlay.walkIconHeight_px)
            {
                // save offset if exiting walking mode
                if (overlay.isWalkingMode())
                {
                    map.savedOffsetX = map.offsetX_norm;
                    map.savedOffsetY = map.offsetY_norm;
                    map.savedViewFraction = true;
                }

                overlay.setWalkingMode(!overlay.isWalkingMode());
                map.viewFraction = overlay.isWalkingMode() ? 0.5f : 1.0f;

                // restore offset if returning to walking mode
                if (overlay.isWalkingMode() && map.savedViewFraction)
                {
                    map.offsetX_norm = map.savedOffsetX;
                    map.offsetY_norm = map.savedOffsetY;
                }

                clickHandled = true;
            }
        }
        else clickHandled = false;

        // R key
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && !rWasPressed)
        {
            overlay.setWalkingMode(!overlay.isWalkingMode());

            if (!map.zoomToggled)
            {
                map.savedViewFraction = map.viewFraction;
                map.savedOffsetX = map.offsetX_norm;
                map.savedOffsetY = map.offsetY_norm;

                map.viewFraction = 1.0f;
                map.offsetX_norm = 0.0f;
                map.offsetY_norm = 0.0f;

                map.zoomToggled = true;
            }
            else
            {
                map.viewFraction = map.savedViewFraction;
                map.offsetX_norm = map.savedOffsetX;
                map.offsetY_norm = map.savedOffsetY;

                map.zoomToggled = false;
            }

            rWasPressed = true;
        }
        else if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE)
        {
            rWasPressed = false;
        }

        // MEASUREMENT POINTS
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            if (!mapClickHandled)
            {
                // ignore if click on walking icon?
                if (!(mouseX >= overlay.walkIconX_px &&
                    mouseX <= overlay.walkIconX_px + overlay.walkIconWidth_px &&
                    mouseY >= overlay.walkIconY_px &&
                    mouseY <= overlay.walkIconY_px + overlay.walkIconHeight_px))
                {
                    if (map.viewFraction == 1.0f) // only if not in walking mode
                    {
                        overlay.addMeasurementPoint(
                            (float)mouseX / fbW,
                            1.0f - (float)mouseY / fbH, // invert y
                            (float)fbW,
                            (float)fbH
                        );
                    }

                    clickHandled = true;
                }
                mapClickHandled = true;
            }
        }
        else
        {
            mapClickHandled = false;
        }



        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glUniform1i(glGetUniformLocation(shaderProgram, "uIgnoreTransform"), false);
        map.bindShaderTransform(shaderProgram, fbW, fbH);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, map.getTextureID());
        glBindVertexArray(map.getVAO());
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        overlay.drawPin(shaderProgram, fbW, fbH);
        overlay.drawWalkIcon(shaderProgram, fbW, fbH);
        overlay.drawMeasurements(shaderProgram, fbW, fbH);

        if (overlay.isWalkingMode())
        {
            overlay.drawFilledRect(120, fbH - 150, 300, 40, 0, 0, 0, fbW, fbH);
            std::string dist = "DISTANCE: " + std::to_string((int)map.walkedDistancePixels) + " px";
            overlay.drawText(dist.c_str(), 150, 140, 1.0f, 1, 1, 1, fbW, fbH);
        }

        // AUTHOR
        overlay.drawFilledRect(fbW - 380 - 20, fbH - 1020 - 20, 380, 45, 0, 0, 0, fbW, fbH);
        std::string author = "ANA SINIK, SV11/2022";
        overlay.drawText(author.c_str(), fbW - 380 - 20 + 15, fbH - 45 - 20 + 45 - 10, 1.0f, 1, 1, 1, fbW, fbH);

        glfwSwapBuffers(window);
        glfwPollEvents();

        while (glfwGetTime() - initFrameTime < 1 / 75.0) {}
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
