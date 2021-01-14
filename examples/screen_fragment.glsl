#version 450 core

layout(location = 0) smooth in vec2 UV;
layout(location = 0) out vec4 gColor;

void main()
{
	gColor.rgb = mix(vec3(UV, 0.0), vec3(0.0), any(greaterThan(UV, vec2(1.0))) || any(lessThan(UV, vec2(0.0))));
	gColor.a = 1.0;
}