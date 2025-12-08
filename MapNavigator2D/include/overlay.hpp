#pragma once
#include <GL/glew.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <string>
#include <map>

class Overlay {
public:
    Overlay(const char* pinPath, const char* walkingIconPath);
    ~Overlay();

    void drawPin(unsigned int shaderProgram, int windowWidth, int windowHeight);
    void drawWalkIcon(unsigned int shaderProgram, int windowWidth, int windowHeight);
    void drawText(const char* text,
        float x, float y,
        float scale,
        float r, float g, float b,
        int winW, int winH);
    void drawFilledRect(float x, float y, float w, float h,
        float r, float g, float b,
        int screenW, int screenH);
    bool loadFont(const char* path, int fontSize);

    struct FTCharacter {
        GLuint TextureID;
        int Width, Height;
        int BearingX, BearingY;
        long Advance;
    };
    std::map<char, FTCharacter> Characters;

    void setWalkingMode(bool b) { walkingMode = b; }
    bool isWalkingMode() const { return walkingMode; }

private:
    bool walkingMode = false;

    unsigned int pinTextureID = 0, walkTextureID = 0;
    unsigned int pinVAO = 0, pinVBO = 0, pinEBO = 0;
    unsigned int walkVAO = 0, walkVBO = 0, walkEBO = 0;

    // text
    unsigned int textVAO = 0, textVBO = 0;
    unsigned int textShader = 0;
    unsigned int whiteTexture = 0;

    float pinSize = 0.05f;
    float iconSize = 0.5f;
};