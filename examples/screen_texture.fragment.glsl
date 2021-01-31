#version 450 core

layout(location = 0) smooth in vec2 UV;
layout(location = 0) out vec4 gColor;

layout(location = 2, binding = 0) uniform sampler2D gTexture;

void main()
{
	gColor = vec4(texture(gTexture, UV).rgb, 1.0);
}