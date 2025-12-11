#include "../include/Overlay.hpp"
#include "../include/util.hpp"
#include <vector>
#include <iostream>
#include <cmath>

Overlay::Overlay(const char* pinPath, const char* walkingIconPath, const char* rulerIconPath) {
    
    iconRenderer.init(pinSize);
    bool ok = iconRenderer.loadTextures(pinPath, walkingIconPath, rulerIconPath);
    if (!ok) 
        std::cerr << "Warning: some icon textures failed to load in Overlay ctor.\n";
    
    textRenderer.init(nullptr);

    loadFont("fonts/arial.ttf", 24);

    lineShader = createShader("shaders/line.vert", "shaders/line.frag");

    glGenVertexArrays(1, &lineVAO);
    glGenBuffers(1, &lineVBO);
    glBindVertexArray(lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 2, NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

Overlay::~Overlay() {

    if (lineVAO) glDeleteVertexArrays(1, &lineVAO);
    if (lineVBO) glDeleteBuffers(1, &lineVBO);
    if (lineShader) glDeleteProgram(lineShader);
    measurementManager.clear();
    Characters.clear();
}

void Overlay::drawPin(unsigned int shaderProgram, int windowWidth, int windowHeight) {
    iconRenderer.drawPin(shaderProgram);
}

void Overlay::drawWalkIcon(unsigned int shaderProgram, int windowWidth, int windowHeight)
{
    iconRenderer.drawWalkIcon(shaderProgram, windowWidth, windowHeight, walkingMode,
        iconX_px, iconY_px, iconWidth_px, iconHeight_px);
}

void Overlay::drawRulerIcon(unsigned int shaderProgram, int windowWidth, int windowHeight)
{
    iconRenderer.drawRulerIcon(shaderProgram, windowWidth, windowHeight, walkingMode,
        iconX_px, iconY_px, iconWidth_px, iconHeight_px);
}

void Overlay::drawText(const char* text, float x, float y, float scale, float r, float g, float b, int winW, int winH)
{
    textRenderer.drawText(text, x, y, scale, r, g, b, winW, winH, nullptr);
}

void Overlay::drawFilledRect(float x, float y, float w, float h, float r, float g, float b, float a, int screenW, int screenH)
{
    textRenderer.drawFilledRect(x, y, w, h, r, g, b, a, screenW, screenH);
}

void Overlay::addMeasurementPoint(float x_norm, float y_norm, float mapWidth, float mapHeight)
{
    measurementManager.addPoint(x_norm, y_norm, mapWidth, mapHeight);
}

bool Overlay::removeMeasurementPointAt(float x_px, float y_px, float mapWidth, float mapHeight)
{
    return measurementManager.removePointAt(x_px, y_px, mapWidth, mapHeight);
}

void Overlay::drawMeasurements(unsigned int shaderProgram, int winW, int winH)
{
    if (walkingMode) return;

    const auto& pts = measurementManager.getPoints();
    for (size_t i = 0; i < pts.size(); i++) {
        float x = pts[i].xNorm * winW;
        float y = pts[i].yNorm * winH;
        drawFilledCircle(x, y, pointRadius, 24, 1.0f, 1.0f, 1.0f, winW, winH);
        if (i > 0) {
            float x0 = pts[i - 1].xNorm * winW;
            float y0 = pts[i - 1].yNorm * winH;
            drawLine(x0, y0, x, y, winW, winH);
        }
    }
}

void Overlay::drawLine(float x1, float y1, float x2, float y2, int windowWidth, int windowHeight)
{
    glUseProgram(lineShader);

    float proj[16] = {
        2.0f / windowWidth, 0, 0, 0,
        0, 2.0f / windowHeight, 0, 0,
        0, 0, 1, 0,
       -1, -1, 0, 1
    };
    glUniformMatrix4fv(glGetUniformLocation(lineShader, "uProjection"), 1, GL_FALSE, proj);
    glUniform3f(glGetUniformLocation(lineShader, "uColor"), 1.0f, 0.0f, 0.0f);

    float thickness = 5.0f;
    float dx = x2 - x1;
    float dy = y2 - y1;
    float len = sqrtf(dx * dx + dy * dy);
    if (len < 0.0001f) return;

    float nx = -dy / len * thickness * 0.5f;
    float ny = dx / len * thickness * 0.5f;

    float verts[6][2] = {
        { x1 + nx, y1 + ny },
        { x2 + nx, y2 + ny },
        { x2 - nx, y2 - ny },

        { x1 + nx, y1 + ny },
        { x2 - nx, y2 - ny },
        { x1 - nx, y1 - ny }
    };

    glBindVertexArray(lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Overlay::drawFilledCircle(float cx, float cy, float radius, int segments, float r, float g, float b, int winW, int winH)
{
    glUseProgram(lineShader);
    float proj[16] = {
        2.0f / (float)winW, 0, 0, 0,
        0, 2.0f / (float)winH, 0, 0,
        0, 0, 1, 0,
       -1, -1, 0, 1
    };
    glUniformMatrix4fv(glGetUniformLocation(lineShader, "uProjection"), 1, GL_FALSE, proj);
    glUniform3f(glGetUniformLocation(lineShader, "uColor"), r, g, b);

    std::vector<float> verts;
    verts.push_back(cx);
    verts.push_back(cy);

    for (int i = 0; i <= segments; i++)
    {
        float theta = 2.0f * 3.1415926f * float(i) / float(segments);
        float x = cx + radius * cosf(theta);
        float y = cy + radius * sinf(theta);
        verts.push_back(x);
        verts.push_back(y);
    }

    glBindVertexArray(lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_DYNAMIC_DRAW);

    glDrawArrays(GL_TRIANGLE_FAN, 0, (GLsizei)(verts.size() / 2));
    glBindVertexArray(0);
}

bool Overlay::loadFont(const char* path, int fontSize)
{
    std::map<char, ::FTCharacterTR> trmap;
    bool ok = textRenderer.loadFont(path, fontSize, nullptr);
    std::map<char, ::FTCharacterTR> outMap;
    ok = textRenderer.loadFont(path, fontSize, &outMap);
    Characters.clear();
    for (auto& p : outMap) {
        FTCharacter oc;
        oc.TextureID = p.second.TextureID;
        oc.Width = p.second.Width;
        oc.Height = p.second.Height;
        oc.BearingX = p.second.BearingX;
        oc.BearingY = p.second.BearingY;
        oc.Advance = p.second.Advance;
        Characters[p.first] = oc;
    }
    return ok;
}

static FTCharacterTR overlayConvert(const ::FTCharacterTR& c)
{
    FTCharacterTR o; o.TextureID = c.TextureID; o.Width = c.Width; o.Height = c.Height; o.BearingX = c.BearingX; o.BearingY = c.BearingY; o.Advance = c.Advance; return o;
}
