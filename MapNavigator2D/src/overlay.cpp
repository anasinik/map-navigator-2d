#include "../include/overlay.hpp"
#include "../include/util.hpp"
#include <vector>
#include <iostream>

static GLuint createWhiteTexture() {
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    unsigned char white = 255;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 1, 1, 0, GL_RED, GL_UNSIGNED_BYTE, &white);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    return tex;
}

Overlay::Overlay(const char* pinPath, const char* walkingIconPath) {
    int texW, texH;
    pinTextureID = loadImageToTexture(pinPath, texW, texH);
    walkTextureID = loadImageToTexture(walkingIconPath, texW, texH);

    float vertices[] = {
        -pinSize,-pinSize, 0.0f,0.0f,
         pinSize,-pinSize, 1.0f,0.0f,
         pinSize, pinSize, 1.0f,1.0f,
        -pinSize, pinSize, 0.0f,1.0f
    };

    // pin icon
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

    // Walk icon
    glGenVertexArrays(1, &walkVAO);
    glGenBuffers(1, &walkVBO);
    glGenBuffers(1, &walkEBO);
    glBindVertexArray(walkVAO);
    glBindBuffer(GL_ARRAY_BUFFER, walkVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, walkEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // Text VAO/VBO
    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);

    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    textShader = createShader("shaders/text.vert", "shaders/text.frag");
    whiteTexture = createWhiteTexture();
}

Overlay::~Overlay() {
    glDeleteVertexArrays(1, &pinVAO);
    glDeleteBuffers(1, &pinVBO);
    glDeleteBuffers(1, &pinEBO);
    glDeleteTextures(1, &pinTextureID);

    glDeleteVertexArrays(1, &walkVAO);
    glDeleteBuffers(1, &walkVBO);
    glDeleteBuffers(1, &walkEBO);
    glDeleteTextures(1, &walkTextureID);

    glDeleteVertexArrays(1, &textVAO);
    glDeleteBuffers(1, &textVBO);
    if (textShader) glDeleteProgram(textShader);
    if (whiteTexture) glDeleteTextures(1, &whiteTexture);
}

void Overlay::drawPin(unsigned int shaderProgram, int windowWidth, int windowHeight) {
    glUseProgram(shaderProgram);

    glUniform1i(glGetUniformLocation(shaderProgram, "uScreenIcon"), true);
    glUniform1i(glGetUniformLocation(shaderProgram, "uIgnoreTransform"), true);
    glUniform2f(glGetUniformLocation(shaderProgram, "uIconOffset"), 0.0f, 0.0f);

    glBindVertexArray(pinVAO);
    glBindTexture(GL_TEXTURE_2D, pinTextureID);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glUniform1i(glGetUniformLocation(shaderProgram, "uScreenIcon"), false);
    glUniform1i(glGetUniformLocation(shaderProgram, "uIgnoreTransform"), false);
}

void Overlay::drawWalkIcon(unsigned int shaderProgram, int windowWidth, int windowHeight)
{
    if (!walkingMode) return;

    glUseProgram(shaderProgram);

    float cmLeft = 2.0f;
    float cmTop = 3.0f;
    float dpi = 96.0f;
    float pxLeft = cmLeft / 2.54f * dpi;
    float pxTop = cmTop / 2.54f * dpi;

    float x = -1.0f + pxLeft / windowWidth * 2.0f; // NDC
    float y = 1.0f - pxTop / windowHeight * 2.0f; // NDC

    glUniform1i(glGetUniformLocation(shaderProgram, "uScreenIcon"), true);
    glUniform1i(glGetUniformLocation(shaderProgram, "uIgnoreTransform"), true);
    glUniform2f(glGetUniformLocation(shaderProgram, "uIconOffset"), x, y);

    glBindVertexArray(walkVAO);
    glBindTexture(GL_TEXTURE_2D, walkTextureID);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glUniform1i(glGetUniformLocation(shaderProgram, "uScreenIcon"), false);
    glUniform1i(glGetUniformLocation(shaderProgram, "uIgnoreTransform"), false);
}

void Overlay::drawText(const char* text,
    float x, float y,
    float scale,
    float r, float g, float b,
    int winW, int winH)
{
    if (!textShader) return;
    if (Characters.empty()) return;

    GLboolean wasBlendEnabled = glIsEnabled(GL_BLEND);
    GLboolean wasDepthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
    GLint oldBlendSrc, oldBlendDst;
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &oldBlendSrc);
    glGetIntegerv(GL_BLEND_DST_ALPHA, &oldBlendDst);

    glUseProgram(textShader);

    glUniform1i(glGetUniformLocation(textShader, "text"), 0);

    float left = 0.0f;
    float right = (float)winW;
    float bottom = 0.0f;
    float top = (float)winH;

    float proj[16] = {
        2.0f / (right - left), 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
        0.0f, 0.0f, -1.0f, 0.0f,
        -(right + left) / (right - left), -(top + bottom) / (top - bottom), 0.0f, 1.0f
    };

    glUniformMatrix4fv(glGetUniformLocation(textShader, "projection"), 1, GL_FALSE, proj);
    glUniform3f(glGetUniformLocation(textShader, "textColor"), r, g, b);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(textVAO);

    float originY = (float)winH - y;

    for (char cc : std::string(text)) {
        if (Characters.count(cc) == 0) {
            continue;
        }

        FTCharacter ch = Characters[cc];

        if (ch.Width == 0 || ch.Height == 0) {
            x += (ch.Advance >> 6) * scale;
            continue;
        }

        float xpos = x + ch.BearingX * scale;
        float ypos = originY - (ch.Height - ch.BearingY) * scale;

        float w = (float)ch.Width * scale;
        float h = (float)ch.Height * scale;

        float verts[6][4] = {
            { xpos,     ypos + h,  0.0f, 0.0f },
            { xpos,     ypos,      0.0f, 1.0f },
            { xpos + w, ypos,      1.0f, 1.0f },

            { xpos,     ypos + h,  0.0f, 0.0f },
            { xpos + w, ypos,      1.0f, 1.0f },
            { xpos + w, ypos + h,  1.0f, 0.0f }
        };

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);

        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += (ch.Advance >> 6) * scale;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    if (!wasBlendEnabled) glDisable(GL_BLEND);
    if (wasDepthTestEnabled) glEnable(GL_DEPTH_TEST);
    glBlendFunc(oldBlendSrc, oldBlendDst);
}

bool Overlay::loadFont(const char* path, int fontSize)
{
    Characters.clear();

    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        std::cout << "Failed to init FreeType.\n";
        return false;
    }

    FT_Face face;
    if (FT_New_Face(ft, path, 0, &face)) {
        std::cout << "Failed to load font.\n";
        FT_Done_FreeType(ft);
        return false;
    }

    FT_Set_Pixel_Sizes(face, 0, fontSize);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 0; c < 128; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cout << "Failed to load glyph " << (int)c << "\n";
            continue;
        }

        int w = face->glyph->bitmap.width;
        int h = face->glyph->bitmap.rows;

        GLuint tex = 0;
        if (w > 0 && h > 0) {
            glGenTextures(1, &tex);
            glBindTexture(GL_TEXTURE_2D, tex);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        FTCharacter ch = FTCharacter();
        ch.TextureID = tex;
        ch.Width = w;
        ch.Height = h;
        ch.BearingX = face->glyph->bitmap_left;
        ch.BearingY = face->glyph->bitmap_top;
        ch.Advance = face->glyph->advance.x;

        Characters[(char)c] = ch;
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    return true;
}

void Overlay::drawFilledRect(float x, float y, float w, float h,
    float r, float g, float b,
    int screenW, int screenH)
{
    if (!textShader) return;

    float verts[6][4] = {
        { x,     y,     0.0f, 0.0f },
        { x + w, y,     0.0f, 0.0f },
        { x + w, y + h, 0.0f, 0.0f },

        { x,     y,     0.0f, 0.0f },
        { x + w, y + h, 0.0f, 0.0f },
        { x,     y + h, 0.0f, 0.0f }
    };

    glUseProgram(textShader);

    float left = 0.0f;
    float right = (float)screenW;
    float bottom = 0.0f;
    float top = (float)screenH;

    float proj[16] = {
        2.0f / (right - left), 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
        0.0f, 0.0f, -1.0f, 0.0f,
        -(right + left) / (right - left), -(top + bottom) / (top - bottom), 0.0f, 1.0f
    };
    glUniformMatrix4fv(glGetUniformLocation(textShader, "projection"), 1, GL_FALSE, proj);

    glUniform3f(glGetUniformLocation(textShader, "textColor"), r, g, b);
    glUniform1i(glGetUniformLocation(textShader, "useTexture"), GL_FALSE);

    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);

    glBindTexture(GL_TEXTURE_2D, whiteTexture);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
