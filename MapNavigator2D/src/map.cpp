#include "../include/map.hpp"
#include "../include/util.hpp"
#include <GLFW/glfw3.h>
#include <sstream>
#include <iostream>

Map::Map(const std::string& texturePath) {
    textureID = loadImageToTexture(texturePath.c_str(), texWidth, texHeight);
    if (textureID == 0) {
        std::cout << "Failed to load map texture\n";
        return;
    }

    std::cout << "Loaded texture size: " << texWidth << " x " << texHeight << std::endl;

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


void Map::draw() {
    glBindTexture(GL_TEXTURE_2D, textureID);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Map::move(float dx, float dy) {
    offsetX += dx;
    offsetY += dy;
    // use for scroll
}

void Map::bindShaderTransform(unsigned int shaderProgram, int windowWidth, int windowHeight)
{
    float windowAspect = (float)windowWidth / windowHeight;
    float mapAspect = (float)texWidth / texHeight;

    float scaleX, scaleY;
    float offsetX = 0.0f, offsetY = 0.0f;

    if (mapAspect > windowAspect) {
        scaleX = 1.0f;
        scaleY = windowAspect / mapAspect;
        offsetY = 0.0f;
    }
    else {
        scaleY = 1.0f;
        scaleX = mapAspect / windowAspect;
        offsetX = 0.0f;
    }

    int locScale = glGetUniformLocation(shaderProgram, "uScale");
    int locOffset = glGetUniformLocation(shaderProgram, "uOffset");

    glUseProgram(shaderProgram);
    glUniform2f(locScale, scaleX, scaleY);
    glUniform2f(locOffset, offsetX, offsetY);
}
