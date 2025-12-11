#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../include/Util.hpp"
#include "../include/Map.hpp"
#include "../include/Overlay.hpp"

static bool mouseInsideIcon(double mx, double my, const Overlay& overlay)
{
    return (mx >= overlay.iconX_px &&
        mx <= overlay.iconX_px + overlay.iconWidth_px &&
        my >= overlay.iconY_px &&
        my <= overlay.iconY_px + overlay.iconHeight_px);
}

static void handleEscape(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

static float computeDelta(double& lastTime)
{
    double now = glfwGetTime();
    float dt = float(now - lastTime);
    lastTime = now;
    return dt;
}

static void handleWalkingMovement(GLFWwindow* window, Map& map, float dt, int fbW, int fbH)
{
    float dx = 0.0f, dy = 0.0f;
    float speed = map.moveSpeedPixels;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) dy -= speed * dt;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) dy += speed * dt;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) dx -= speed * dt;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) dx += speed * dt;

    map.walkedDistancePixels += map.applyMovementAndMeasure(dx, dy, fbW, fbH);
}

static void toggleWalkingMode(Overlay& overlay, Map& map)
{
    bool entering = !overlay.isWalkingMode();

    if (!entering)  // exiting walking mode
    {
        map.savedOffsetX = map.offsetX_norm;
        map.savedOffsetY = map.offsetY_norm;

        map.viewFraction = 1.0f;
        overlay.setWalkingMode(false);
        return;
    }

    // entering walking mode
    overlay.setWalkingMode(true);
    map.viewFraction = 0.5f;

    map.offsetX_norm = map.savedOffsetX;
    map.offsetY_norm = map.savedOffsetY;
}



static void handleIconClick(Overlay& overlay, Map& map, bool& clickHandled)
{
    if (!clickHandled) {
        toggleWalkingMode(overlay, map);
        clickHandled = true;
    }
}

static void toggleZoom(Map& map)
{
    if (map.zoomToggled)
    {
        map.savedOffsetX = map.offsetX_norm;
        map.savedOffsetY = map.offsetY_norm;

        map.viewFraction = 1.0f;
        map.offsetX_norm = 0.0f;
        map.offsetY_norm = 0.0f;
        map.zoomToggled = true;
    }
    else
    {
        map.offsetX_norm = map.savedOffsetX;
        map.offsetY_norm = map.savedOffsetY;
        map.zoomToggled = false;
    }
}

static void handleRkey(GLFWwindow* window, Map& map, Overlay& overlay, bool& rWasPressed)
{
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && !rWasPressed)
    {
        toggleZoom(map);
        toggleWalkingMode(overlay, map);

        rWasPressed = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE)
        rWasPressed = false;
}



static void handleMeasurementClick(
    Overlay& overlay, Map& map, double mx, double my,
    int fbW, int fbH, bool& clickHandled, bool& mapClickHandled)
{
    if (map.viewFraction != 1.0f) return;  // Only allowed in full mode

    if (!mapClickHandled)
    {
        bool removed = overlay.removeMeasurementPointAt(mx, my, (float)fbW, (float)fbH);
        if (!removed)
        {
            overlay.addMeasurementPoint(
                float(mx) / fbW,
                1.0f - float(my) / fbH,
                (float)fbW, (float)fbH
            );
        }

        clickHandled = true;
        mapClickHandled = true;
    }
}

int main()
{
    if (!glfwInit()) return endProgram("GLFW init failed");

    // Window setup
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primary);

    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "MapNavigator2D", primary, NULL);
    if (!window) return endProgram("Window creation failed");

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK)
        return endProgram("GLEW init failed");

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.757f, 0.761f, 0.753f, 1.0f);

    // Resources
    unsigned int shaderProgram = createShader("shaders/vertex_shader.vert", "shaders/fragment_shader.frag");
    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "uTexture"), 0);

    Map map("textures/novi-sad-map.jpg");

    Overlay overlay("textures/pin.png", "textures/walking_icon.png", "textures/ruler.png");
    overlay.loadFont("fonts/arial.ttf", 24);

    map.viewFraction = 0.5f;
    overlay.setWalkingMode(true);

    map.offsetX_norm = 0.5f - map.viewFraction / 2.0f;
    map.offsetY_norm = 0.5f - map.viewFraction / 2.0f;

    double lastTime = glfwGetTime();
    bool clickHandled = false;
    bool mapClickHandled = false;
    bool rWasPressed = false;

    int fbW, fbH;
    glfwGetFramebufferSize(window, &fbW, &fbH);

    while (!glfwWindowShouldClose(window))
    {
        handleEscape(window);

        GLFWcursor* compass = loadImageToCursor("textures/compas.png");
        glfwSetCursor(window, compass);

        float dt = computeDelta(lastTime);

        if (overlay.isWalkingMode())
            handleWalkingMovement(window, map, dt, fbW, fbH);

        // Input
        double mx, my;
        glfwGetCursorPos(window, &mx, &my);

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            if (mouseInsideIcon(mx, my, overlay))
                handleIconClick(overlay, map, clickHandled);
            else
                handleMeasurementClick(overlay, map, mx, my, fbW, fbH, clickHandled, mapClickHandled);
        }
        else
        {
            clickHandled = false;
            mapClickHandled = false;
        }

        handleRkey(window, map, overlay, rWasPressed);

        // Draw
        glViewport(0, 0, fbW, fbH);
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
        overlay.drawRulerIcon(shaderProgram, fbW, fbH);
        overlay.drawMeasurements(shaderProgram, fbW, fbH);

        overlay.drawFilledRect(120, fbH - 150, 300, 40, 0, 0, 0, 0.5f, fbW, fbH);

        if (overlay.isWalkingMode())
            overlay.drawText(("DISTANCE: " + std::to_string((int)map.walkedDistancePixels)).c_str(),
                150, 140, 1, 1, 1, 1, fbW, fbH);
        else
            overlay.drawText(("TOTAL: " + std::to_string((int)overlay.getTotalMeasuredDistance())).c_str(),
                150, 140, 1, 1, 1, 1, fbW, fbH);

        // Author
        overlay.drawFilledRect(fbW - 400, fbH - 1040, 380, 45, 0, 0, 0, 0.5f, fbW, fbH);
        overlay.drawText("ANA SINIK, SV11/2022", fbW - 380, fbH - 45, 1, 1, 1, 1, fbW, fbH);

        glfwSwapBuffers(window);
        glfwPollEvents();

        while (glfwGetTime() - lastTime < 1 / 75.0) {}
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
