#pragma once
#include <GL/glew.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <string>
#include <map>
#include <vector>
#include "../include/point.hpp"

#include "IconRenderer.hpp"
#include "TextRenderer.hpp"
#include "MeasurementManager.hpp"

class Overlay {
public:
    Overlay(const char* pinPath, const char* walkingIconPath, const char* rulerIconPath);
    ~Overlay();

    void drawPin(unsigned int shaderProgram, int windowWidth, int windowHeight);
    void drawWalkIcon(unsigned int shaderProgram, int windowWidth, int windowHeight);
    void drawRulerIcon(unsigned int shaderProgram, int windowWidth, int windowHeight);
    void drawText(const char* text,
        float x, float y,
        float scale,
        float r, float g, float b,
        int winW, int winH);
    void drawFilledRect(float x, float y, float w, float h,
        float r, float g, float b, float a,
        int screenW, int screenH);
    bool loadFont(const char* path, int fontSize);
    void drawLine(float x1, float y1, float x2, float y2, int windowWidth, int windowHeight);
    void drawFilledCircle(float cx, float cy, float radius, int segments, float r, float g, float b, int winW, int winH);

    struct FTCharacter {
        GLuint TextureID;
        int Width, Height;
        int BearingX, BearingY;
        long Advance;
    };
    std::map<char, FTCharacter> Characters;

    void setWalkingMode(bool b) { walkingMode = b; }
    bool isWalkingMode() const { return walkingMode; }

    float iconX_px = 0.0f;
    float iconY_px = 0.0f;
    float iconWidth_px = 0.0f;
    float iconHeight_px = 0.0f;

    void addMeasurementPoint(float x_norm, float y_norm, float mapWidth, float mapHeight);
    bool removeMeasurementPointAt(float x_px, float y_px, float mapWidth, float mapHeight);

    void drawMeasurements(unsigned int shaderProgram, int winW, int winH);
    void setMeasurementMode(bool b) { measurementMode = b; }
    bool isMeasurementMode() const { return measurementMode; }
    float getTotalMeasuredDistance() const { return measurementManager.getTotalDistance(); }
    void setTotalMeasuredDistance(float total) { measurementManager.setTotalDistance(total); }
    std::vector<MeasurementPoint> getMeasurementPoints() const { return measurementManager.getPoints(); }

    unsigned int lineShader = 0;
    unsigned int lineVAO = 0, lineVBO = 0;

private:
    bool walkingMode = false;

    IconRenderer iconRenderer;
    TextRenderer textRenderer;
    MeasurementManager measurementManager;

    unsigned int pinTextureID = 0, walkTextureID = 0, rulerTextureID = 0;
    unsigned int pinVAO = 0, pinVBO = 0, pinEBO = 0;
    unsigned int walkVAO = 0, walkVBO = 0, walkEBO = 0;
    unsigned int rulerVAO = 0, rulerVBO = 0, rulerEBO = 0;

    unsigned int textVAO = 0, textVBO = 0;
    unsigned int textShader = 0;
    unsigned int whiteTexture = 0;

    float pinSize = 0.05f;
    float iconSize = 0.5f;

    bool measurementMode = false;
    float pointRadius = 6.0f;
};
