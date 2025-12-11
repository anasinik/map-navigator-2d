#include "../include/Map.hpp"
#include "../include/util.hpp"
#include "../include/overlay.hpp"

#include <GLFW/glfw3.h>
#include <sstream>
#include <iostream>

Map::Map(const std::string& texturePath) {
    textureID = loadImageToTexture(texturePath.c_str(), texWidth, texHeight);

    float vertices[] = {
        // positions    // tex coords
        -1.0f, -1.0f,   0.0f, 0.0f,
         1.0f, -1.0f,   1.0f, 0.0f,
         1.0f,  1.0f,   1.0f, 1.0f,
        -1.0f,  1.0f,   0.0f, 1.0f
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

Map::~Map() {
    if (textureID != 0) glDeleteTextures(1, &textureID);
    if (VAO != 0) glDeleteVertexArrays(1, &VAO);
    if (VBO != 0) glDeleteBuffers(1, &VBO);
    if (EBO != 0) glDeleteBuffers(1, &EBO);
}

void Map::draw() {
    glBindTexture(GL_TEXTURE_2D, textureID);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Map::movePixels(float dx, float dy) {
    float dxNorm = dx / (float)texWidth;
    float dyNorm = dy / (float)texHeight;

    offsetX_norm += dxNorm;
    offsetY_norm += dyNorm;
}


void Map::bindShaderTransform(unsigned int shaderProgram, int windowWidth, int windowHeight)
{
    float mapAspect = (float)texWidth / texHeight;
    float windowAspect = (float)windowWidth / windowHeight;

    float mapZoom = viewFraction;

    float zoomX, zoomY;

    if (mapAspect > windowAspect) {
        zoomX = mapZoom;
        zoomY = mapZoom * (windowAspect / mapAspect);
    }

    else {
        zoomY = mapZoom;
        zoomX = mapZoom * (mapAspect / windowAspect);
    }

    float minX = zoomX / 2.0f;
    float maxX = 1.0f - zoomX / 2.0f;

    float minY = zoomY / 2.0f;
    float maxY = 1.0f - zoomY / 2.0f;

    if (offsetX_norm < minX) offsetX_norm = minX;
    if (offsetX_norm > maxX) offsetX_norm = maxX;
    if (offsetY_norm < minY) offsetY_norm = minY;
    if (offsetY_norm > maxY) offsetY_norm = maxY;

    glUseProgram(shaderProgram);
    glUniform2f(glGetUniformLocation(shaderProgram, "uScale"), zoomX, zoomY);
    glUniform2f(glGetUniformLocation(shaderProgram, "uOffset"), offsetX_norm, offsetY_norm);
}

void Map::setViewFraction(float vf) {
    viewFraction = vf;
}

double Map::applyMovementAndMeasure(float dxPix, float dyPix, int windowWidth, int windowHeight)
{
    if (dxPix == 0.0f && dyPix == 0.0f)
        return 0.0;

    float oldX = offsetX_norm;
    float oldY = offsetY_norm;

    float dxNorm = dxPix / (float)texWidth;
    float dyNorm = dyPix / (float)texHeight;

    offsetX_norm -= dxNorm;
    offsetY_norm -= dyNorm;

    float mapAspect = (float)texWidth / texHeight;
    float windowAspect = (float)windowWidth / windowHeight;

    float zoomX, zoomY;

    if (mapAspect > windowAspect) {
        zoomX = viewFraction;
        zoomY = viewFraction * (windowAspect / mapAspect);
    }
    else {
        zoomY = viewFraction;
        zoomX = viewFraction * (mapAspect / windowAspect);
    }

    float minX = zoomX / 2.0f;
    float maxX = 1.0f - zoomX / 2.0f;
    float minY = zoomY / 2.0f;
    float maxY = 1.0f - zoomY / 2.0f;

    if (offsetX_norm < minX) offsetX_norm = minX;
    if (offsetX_norm > maxX) offsetX_norm = maxX;
    if (offsetY_norm < minY) offsetY_norm = minY;
    if (offsetY_norm > maxY) offsetY_norm = maxY;

    savedOffsetX = offsetX_norm;
    savedOffsetY = offsetY_norm;

    float deltaX_norm = offsetX_norm - oldX;
    float deltaY_norm = offsetY_norm - oldY;

    float movedX_pix = deltaX_norm * texWidth;
    float movedY_pix = deltaY_norm * texHeight;

    return sqrt(movedX_pix * movedX_pix + movedY_pix * movedY_pix);
}
