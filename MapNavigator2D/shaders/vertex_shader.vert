#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;

uniform bool uScreenIcon;
uniform vec2 uIconOffset;

out vec2 TexCoord;

void main()
{
    vec2 pos = aPos;
    
    if (uScreenIcon)
    {
        pos += uIconOffset;
    }
    gl_Position = vec4(pos, 0.0, 1.0);
    TexCoord = aTexCoord;
}
