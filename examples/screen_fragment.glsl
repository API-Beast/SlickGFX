#version 450 core

layout(location = 0) smooth in vec2 UV;
layout(location = 0) out vec4 gColor;

void main()
{
	gColor.rg = UV;
	gColor.a = 1.0;
}