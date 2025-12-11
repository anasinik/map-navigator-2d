#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D text;
uniform vec4 textColor;
uniform bool useTexture;

void main()
{
    if (useTexture) {
        float alpha = texture(text, TexCoord).r;
        FragColor = vec4(textColor.rgb, alpha);
    } else {
        FragColor = textColor;
    }
}