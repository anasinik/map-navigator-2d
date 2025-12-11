#include "../include/IconRenderer.hpp"
#include "../include/util.hpp"
#include <iostream>

void IconRenderer::initVAO(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO, float pinSize)
{
    float vertices[] = {
        -pinSize,-pinSize, 0.0f,0.0f,
         pinSize,-pinSize, 1.0f,0.0f,
         pinSize, pinSize, 1.0f,1.0f,
        -pinSize, pinSize, 0.0f,1.0f
    };
    unsigned int indices[] = { 0,1,2, 2,3,0 };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

void IconRenderer::init(float pinSize) {
    this->pinSize = pinSize;
    initVAO(pinVAO, pinVBO, pinEBO, pinSize);
    initVAO(walkVAO, walkVBO, walkEBO, pinSize);
    initVAO(rulerVAO, rulerVBO, rulerEBO, pinSize);
}

bool IconRenderer::loadTextures(const char* pinPath, const char* walkPath, const char* rulerPath) {
    int texW, texH;
    pinTexture = loadImageToTexture(pinPath, texW, texH);
    walkTexture = loadImageToTexture(walkPath, texW, texH);
    rulerTexture = loadImageToTexture(rulerPath, texW, texH);
    return pinTexture && walkTexture && rulerTexture;
}

IconRenderer::~IconRenderer()
{
    if (pinTexture) glDeleteTextures(1, &pinTexture);
    if (walkTexture) glDeleteTextures(1, &walkTexture);
    if (rulerTexture) glDeleteTextures(1, &rulerTexture);

    if (pinVAO) glDeleteVertexArrays(1, &pinVAO);
    if (pinVBO) glDeleteBuffers(1, &pinVBO);
    if (pinEBO) glDeleteBuffers(1, &pinEBO);

    if (walkVAO) glDeleteVertexArrays(1, &walkVAO);
    if (walkVBO) glDeleteBuffers(1, &walkVBO);
    if (walkEBO) glDeleteBuffers(1, &walkEBO);

    if (rulerVAO) glDeleteVertexArrays(1, &rulerVAO);
    if (rulerVBO) glDeleteBuffers(1, &rulerVBO);
    if (rulerEBO) glDeleteBuffers(1, &rulerEBO);
}

void IconRenderer::drawPin(unsigned int shaderProgram)
{
    glUseProgram(shaderProgram);

    glUniform1i(glGetUniformLocation(shaderProgram, "uScreenIcon"), true);
    glUniform1i(glGetUniformLocation(shaderProgram, "uIgnoreTransform"), true);
    glUniform2f(glGetUniformLocation(shaderProgram, "uIconOffset"), 0.0f, 0.0f);

    glBindVertexArray(pinVAO);
    glBindTexture(GL_TEXTURE_2D, pinTexture);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glUniform1i(glGetUniformLocation(shaderProgram, "uScreenIcon"), false);
    glUniform1i(glGetUniformLocation(shaderProgram, "uIgnoreTransform"), false);
}

void IconRenderer::drawWalkIcon(unsigned int shaderProgram, int windowWidth, int windowHeight,
    bool walkingMode,
    float& outIconX_px, float& outIconY_px,
    float& outIconWidth_px, float& outIconHeight_px)
{
    if (!walkingMode) return;

    glUseProgram(shaderProgram);

    float cmLeft = 2.0f;
    float cmTop = 3.0f;
    float dpi = 96.0f;

    float pxLeft = cmLeft / 2.54f * dpi;
    float pxTop = cmTop / 2.54f * dpi;

    float iconWidth = 32.0f;
    float iconHeight = 32.0f;

    float x = -1.0f + pxLeft / windowWidth * 2.0f;
    float y = 1.0f - pxTop / windowHeight * 2.0f;

    outIconX_px = pxLeft - iconWidth / 2.0f;
    outIconY_px = pxTop - iconHeight / 2.0f;
    outIconWidth_px = iconWidth;
    outIconHeight_px = iconHeight;

    float padding = 5.0f;
    outIconX_px -= padding;
    outIconY_px -= padding;
    outIconWidth_px += 2 * padding;
    outIconHeight_px += 2 * padding;

    glUniform1i(glGetUniformLocation(shaderProgram, "uScreenIcon"), true);
    glUniform1i(glGetUniformLocation(shaderProgram, "uIgnoreTransform"), true);
    glUniform2f(glGetUniformLocation(shaderProgram, "uIconOffset"), x, y);

    glBindVertexArray(walkVAO);
    glBindTexture(GL_TEXTURE_2D, walkTexture);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glUniform1i(glGetUniformLocation(shaderProgram, "uScreenIcon"), false);
    glUniform1i(glGetUniformLocation(shaderProgram, "uIgnoreTransform"), false);
}

void IconRenderer::drawRulerIcon(unsigned int shaderProgram, int windowWidth, int windowHeight,
    bool walkingMode,
    float& outIconX_px, float& outIconY_px,
    float& outIconWidth_px, float& outIconHeight_px)
{
    if (walkingMode) return;

    glUseProgram(shaderProgram);

    float cmLeft = 2.0f;
    float cmTop = 3.0f;
    float dpi = 96.0f;

    float pxLeft = cmLeft / 2.54f * dpi;
    float pxTop = cmTop / 2.54f * dpi;

    float iconWidth = 32.0f;
    float iconHeight = 32.0f;

    float x = -1.0f + pxLeft / windowWidth * 2.0f;
    float y = 1.0f - pxTop / windowHeight * 2.0f;

    outIconX_px = pxLeft - iconWidth / 2.0f;
    outIconY_px = pxTop - iconHeight / 2.0f;
    outIconWidth_px = iconWidth;
    outIconHeight_px = iconHeight;

    float padding = 5.0f;
    outIconX_px -= padding;
    outIconY_px -= padding;
    outIconWidth_px += 2 * padding;
    outIconHeight_px += 2 * padding;

    glUniform1i(glGetUniformLocation(shaderProgram, "uScreenIcon"), true);
    glUniform1i(glGetUniformLocation(shaderProgram, "uIgnoreTransform"), true);
    glUniform2f(glGetUniformLocation(shaderProgram, "uIconOffset"), x, y);

    glBindVertexArray(rulerVAO);
    glBindTexture(GL_TEXTURE_2D, rulerTexture);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glUniform1i(glGetUniformLocation(shaderProgram, "uScreenIcon"), false);
    glUniform1i(glGetUniformLocation(shaderProgram, "uIgnoreTransform"), false);
}
