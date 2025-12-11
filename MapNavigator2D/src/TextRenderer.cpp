#include "../include/TextRenderer.hpp"
#include "../include/util.hpp"
#include <iostream>

GLuint TextRenderer::createWhiteTexture()
{
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

void TextRenderer::ensureInitialized()
{
    if (initialized)
        return;

    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);

    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glBindVertexArray(0);

    textShader = createShader("shaders/text.vert", "shaders/text.frag");
    whiteTexture = createWhiteTexture();

    initialized = true;
}

void TextRenderer::init(std::map<char, FTCharacterTR> *outCharacters)
{
    ensureInitialized();
    if (outCharacters)
    {
        *outCharacters = localChars;
    }
}

TextRenderer::~TextRenderer()
{
    if (textVAO)
        glDeleteVertexArrays(1, &textVAO);
    if (textVBO)
        glDeleteBuffers(1, &textVBO);
    if (textShader)
        glDeleteProgram(textShader);
    if (whiteTexture)
        glDeleteTextures(1, &whiteTexture);

    for (auto &p : localChars)
    {
        if (p.second.TextureID)
            glDeleteTextures(1, &p.second.TextureID);
    }
}

bool TextRenderer::loadFont(const char *path, int fontSize, std::map<char, FTCharacterTR> *outCharacters)
{
    ensureInitialized();

    for (auto &p : localChars)
    {
        if (p.second.TextureID)
            glDeleteTextures(1, &p.second.TextureID);
    }
    localChars.clear();
    if (outCharacters)
        outCharacters->clear();

    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "Failed to init FreeType.\n";
        return false;
    }

    FT_Face face;
    if (FT_New_Face(ft, path, 0, &face))
    {
        std::cout << "Failed to load font.\n";
        FT_Done_FreeType(ft);
        return false;
    }

    FT_Set_Pixel_Sizes(face, 0, fontSize);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 0; c < 128; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "Failed to load glyph " << (int)c << "\n";
            continue;
        }

        int w = face->glyph->bitmap.width;
        int h = face->glyph->bitmap.rows;

        GLuint tex = 0;
        if (w > 0 && h > 0)
        {
            glGenTextures(1, &tex);
            glBindTexture(GL_TEXTURE_2D, tex);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        FTCharacterTR ch;
        ch.TextureID = tex;
        ch.Width = w;
        ch.Height = h;
        ch.BearingX = face->glyph->bitmap_left;
        ch.BearingY = face->glyph->bitmap_top;
        ch.Advance = face->glyph->advance.x;

        localChars[(char)c] = ch;
        if (outCharacters)
            (*outCharacters)[(char)c] = ch;
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    return true;
}

void TextRenderer::drawText(const char *text, float x, float y, float scale, float r, float g, float b, int winW, int winH, const std::map<char, FTCharacterTR> *characters)
{
    const std::map<char, FTCharacterTR> *charsPtr = characters ? characters : &localChars;
    if (charsPtr->empty())
        return;

    GLboolean wasBlendEnabled = glIsEnabled(GL_BLEND);
    GLboolean wasDepthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
    GLint oldBlendSrc, oldBlendDst;
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &oldBlendSrc);
    glGetIntegerv(GL_BLEND_DST_ALPHA, &oldBlendDst);

    glUseProgram(textShader);

    glUniform1i(glGetUniformLocation(textShader, "text"), 0);
    glUniform1i(glGetUniformLocation(textShader, "useTexture"), GL_TRUE);

    float left = 0.0f;
    float right = (float)winW;
    float bottom = 0.0f;
    float top = (float)winH;

    float proj[16] = {
        2.0f / (right - left), 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
        0.0f, 0.0f, -1.0f, 0.0f,
        -(right + left) / (right - left), -(top + bottom) / (top - bottom), 0.0f, 1.0f};

    glUniformMatrix4fv(glGetUniformLocation(textShader, "projection"), 1, GL_FALSE, proj);
    glUniform3f(glGetUniformLocation(textShader, "textColor"), r, g, b);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(textVAO);

    float originY = (float)winH - y;

    for (char cc : std::string(text))
    {
        if (charsPtr->count(cc) == 0)
        {
            continue;
        }

        FTCharacterTR ch = (*charsPtr).at(cc);

        if (ch.Width == 0 || ch.Height == 0)
        {
            x += (ch.Advance >> 6) * scale;
            continue;
        }

        float xpos = x + ch.BearingX * scale;
        float ypos = originY - (ch.Height - ch.BearingY) * scale;

        float w = (float)ch.Width * scale;
        float h = (float)ch.Height * scale;

        float verts[6][4] = {
            {xpos, ypos + h, 0.0f, 0.0f},
            {xpos, ypos, 0.0f, 1.0f},
            {xpos + w, ypos, 1.0f, 1.0f},

            {xpos, ypos + h, 0.0f, 0.0f},
            {xpos + w, ypos, 1.0f, 1.0f},
            {xpos + w, ypos + h, 1.0f, 0.0f}};

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);

        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += (ch.Advance >> 6) * scale;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    if (!wasBlendEnabled)
        glDisable(GL_BLEND);
    if (wasDepthTestEnabled)
        glEnable(GL_DEPTH_TEST);
    glBlendFunc(oldBlendSrc, oldBlendDst);
}

void TextRenderer::drawFilledRect(float x, float y, float w, float h,
                                  float r, float g, float b, float a,
                                  int screenW, int screenH)
{
    ensureInitialized();

    float verts[6][4] = {
        {x, y, 0.0f, 0.0f},
        {x + w, y, 0.0f, 0.0f},
        {x + w, y + h, 0.0f, 0.0f},

        {x, y, 0.0f, 0.0f},
        {x + w, y + h, 0.0f, 0.0f},
        {x, y + h, 0.0f, 0.0f}};

    glUseProgram(textShader);

    float left = 0.0f;
    float right = (float)screenW;
    float bottom = 0.0f;
    float top = (float)screenH;

    float proj[16] = {
        2.0f / (right - left), 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
        0.0f, 0.0f, -1.0f, 0.0f,
        -(right + left) / (right - left), -(top + bottom) / (top - bottom), 0.0f, 1.0f};
    glUniformMatrix4fv(glGetUniformLocation(textShader, "projection"), 1, GL_FALSE, proj);

    glUniform4f(glGetUniformLocation(textShader, "textColor"), r, g, b, a);
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
