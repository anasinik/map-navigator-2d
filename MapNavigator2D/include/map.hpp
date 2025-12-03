#include <sstream>
#include <iostream>
#include <string>
using namespace std;

class Map {
private:
    unsigned int VAO, VBO, EBO;
    unsigned int textureID;

    int texWidth = 0;
    int texHeight = 0;

    float offsetX = 0.0f;
    float offsetY = 0.0f;

public:
    Map(const string& texturePath);

    void draw();
    void move(float dx, float dy);

    void bindShaderTransform(unsigned int shaderProgram,
        int windowWidth, int windowHeight);

    unsigned int getTextureID() const { return textureID; }
    unsigned int getVAO() const { return VAO; }
};
