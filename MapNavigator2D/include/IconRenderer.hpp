#pragma once
#include <GL/glew.h>

class IconRenderer {
public:
    IconRenderer() = default;
    ~IconRenderer();

    void init(float pinSize);
    bool loadTextures(const char* pinPath, const char* walkPath, const char* rulerPath);

    void drawPin(unsigned int shaderProgram);
    void drawWalkIcon(unsigned int shaderProgram, int windowWidth, int windowHeight,
        bool walkingMode,
        float& outIconX_px, float& outIconY_px,
        float& outIconWidth_px, float& outIconHeight_px);
    void drawRulerIcon(unsigned int shaderProgram, int windowWidth, int windowHeight,
        bool walkingMode,
        float& outIconX_px, float& outIconY_px,
        float& outIconWidth_px, float& outIconHeight_px);

private:
    void initVAO(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO, float pinSize);

    unsigned int pinTexture = 0, walkTexture = 0, rulerTexture = 0;
    unsigned int pinVAO = 0, pinVBO = 0, pinEBO = 0;
    unsigned int walkVAO = 0, walkVBO = 0, walkEBO = 0;
    unsigned int rulerVAO = 0, rulerVBO = 0, rulerEBO = 0;

    float pinSize = 0.05f;
};
