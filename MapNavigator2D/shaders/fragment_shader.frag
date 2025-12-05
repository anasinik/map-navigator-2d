#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D uTexture;
uniform vec2 uScale;
uniform vec2 uOffset;
uniform bool uIgnoreTransform;

void main()
{
	vec2 uv;
	if (uIgnoreTransform)
		uv = TexCoord; 
	else
		uv = uOffset + (TexCoord - vec2(0.5)) * uScale;
	
	if (!uIgnoreTransform) {
		if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0) discard;
	}

	FragColor = texture(uTexture, uv);
}