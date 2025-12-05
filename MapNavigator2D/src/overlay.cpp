#include "../include/overlay.hpp"
#include "../include/util.hpp"

Overlay::Overlay(const char* pinPath) {
    int texW, texH;
    pinTextureID = loadImageToTexture(pinPath, texW, texH);

    float halfSize = pinSize;
    float vertices[] = {
        -halfSize, -halfSize, 0.0f, 0.0f,
         halfSize, -halfSize, 1.0f, 0.0f,
         halfSize,  halfSize, 1.0f, 1.0f,
        -halfSize,  halfSize, 0.0f, 1.0f
    };

    unsigned int indices[] = { 0,1,2, 2,3,0 };

    glGenVertexArrays(1, &pinVAO);
    glGenBuffers(1, &pinVBO);
    glGenBuffers(1, &pinEBO);

    glBindVertexArray(pinVAO);

    glBindBuffer(GL_ARRAY_BUFFER, pinVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pinEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

Overlay::~Overlay() {
    glDeleteVertexArrays(1, &pinVAO);
    glDeleteBuffers(1, &pinVBO);
    glDeleteBuffers(1, &pinEBO);
    glDeleteTextures(1, &pinTextureID);
}

void Overlay::drawPin(unsigned int shaderProgram, int windowWidth, int windowHeight) {
    glUseProgram(shaderProgram);

    float aspect = (float)windowWidth / windowHeight;
    float halfSizeX = pinSize;
    float halfSizeY = pinSize / aspect;

    float vertices[] = {
        -halfSizeX, -halfSizeY, 0.0f, 0.0f,
         halfSizeX, -halfSizeY, 1.0f, 0.0f,
         halfSizeX,  halfSizeY, 1.0f, 1.0f,
        -halfSizeX,  halfSizeY, 0.0f, 1.0f
    };

    glBindBuffer(GL_ARRAY_BUFFER, pinVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);


    glUniform1i(glGetUniformLocation(shaderProgram, "uIgnoreTransform"), true);
    glUniform2f(glGetUniformLocation(shaderProgram, "uScale"), 1.0f, 1.0f);
    glUniform2f(glGetUniformLocation(shaderProgram, "uOffset"), 0.0f, 0.0f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, pinTextureID);
    glBindVertexArray(pinVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
