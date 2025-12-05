#pragma once
#include <GL/glew.h>

class Overlay {
public:
    Overlay(const char* pinPath);
    ~Overlay();

    void drawPin(unsigned int shaderProgram, int windowWidth, int windowHeight);

private:
    unsigned int pinTextureID;
    unsigned int pinVAO, pinVBO, pinEBO;
    float pinSize = 0.05f;
};
