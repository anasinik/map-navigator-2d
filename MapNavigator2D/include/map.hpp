#include <sstream>
#include <iostream>
#include <string>
using namespace std;

class Map {
private:
    unsigned int textureID, VAO, VBO, EBO;
    int texWidth = 0, texHeight = 0;
    float offsetX = 0.0f, offsetY = 0.0f;

public:
    Map(const string& texturePath);
    ~Map();

    void draw();
    void movePixels(float dx, float dy);

    void bindShaderTransform(unsigned int shaderProgram, int windowWidth, int windowHeight);

    unsigned int getTextureID() const { return textureID; }
    unsigned int getVAO() const { return VAO; }
    int getTexWidth() const { return texWidth; }
    int getTexHeight() const { return texHeight; }
    void setViewFraction(float vf);
    double applyMovementAndMeasure(float dxPix, float dyPix, int windowWidth, int windowHeight);
    
    float offsetX_norm = 0.0f;
    float offsetY_norm = 0.0f;
    float viewFraction = 0.5f;
    float moveSpeedPixels = 600.0f;
    double walkedDistancePixels = 0.0;

    float savedOffsetX = 0.5f;
    float savedOffsetY = 0.5f;
    bool zoomToggled = false;
};
