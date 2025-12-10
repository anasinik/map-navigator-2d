#pragma once
#include <GL/glew.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <string>
#include <map>
#include <vector>
#include "../include/point.hpp"

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
    void drawLine(float x1, float y1, float x2, float y2, int windowWidth, int windowHeight);

    struct FTCharacter {
        GLuint TextureID;
        int Width, Height;
        int BearingX, BearingY;
        long Advance;
    };
    std::map<char, FTCharacter> Characters;

    void setWalkingMode(bool b) { walkingMode = b; }
    bool isWalkingMode() const { return walkingMode; }

    float walkIconX_px;
    float walkIconY_px;
    float walkIconWidth_px; 
    float walkIconHeight_px;

    void addMeasurementPoint(float x_norm, float y_norm, float mapWidth, float mapHeight);
    void removeMeasurementPointAt(float x_px, float y_px, float mapWidth, float mapHeight);

    void drawMeasurements(unsigned int shaderProgram, int winW, int winH);
    void setMeasurementMode(bool b) { measurementMode = b; }
    bool isMeasurementMode() const { return measurementMode; }
    float getTotalMeasuredDistance() const { return totalMeasuredDistance; }
    void setTotalMeasuredDistance(float total) { this->totalMeasuredDistance = total; }
    std::vector<MeasurementPoint> getMeasurementPoints() const { return measurementPoints; }

    unsigned int lineShader = 0;
    unsigned int lineVAO = 0, lineVBO = 0;
 
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

    std::vector<MeasurementPoint> measurementPoints;
    float totalMeasuredDistance = 0.0f;

    bool measurementMode = false;
    float pointRadius = 6.0f;
};