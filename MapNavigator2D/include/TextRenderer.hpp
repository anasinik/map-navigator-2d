#pragma once
#include <GL/glew.h>
#include <map>
#include <string>
#include <vector>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "../include/point.hpp"

struct FTCharacterTR {
    GLuint TextureID;
    int Width, Height;
    int BearingX, BearingY;
    long Advance;
};

class TextRenderer {
public:
    TextRenderer() = default;
    ~TextRenderer();

    void init(std::map<char, FTCharacterTR>* outCharacters = nullptr);
    bool loadFont(const char* path, int fontSize, std::map<char, FTCharacterTR>* outCharacters = nullptr);

    void drawText(const char* text,
        float x, float y,
        float scale,
        float r, float g, float b,
        int winW, int winH,
        const std::map<char, FTCharacterTR>* characters = nullptr);

    void drawFilledRect(float x, float y, float w, float h,
        float r, float g, float b, float a,
        int screenW, int screenH);

private:
    GLuint textVAO = 0, textVBO = 0;
    GLuint textShader = 0;
    GLuint whiteTexture = 0;

    std::map<char, FTCharacterTR> localChars;

    static GLuint createWhiteTexture();
    void ensureInitialized();
    bool initialized = false;
};
