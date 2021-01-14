#version 450 core

layout(location = 0) out vec2 gWorldUV;

layout(location = 0) uniform vec2 gScreenSize = vec2(800, 600);

void main()
{
	// This creates an oversized triangle whose visible portion spans the entire buffer
	vec2 snorm_uv = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2);
	gl_Position = vec4(snorm_uv * 2.0f + -1.0f, 0.0f, 1.0f);

	float eigenv = 1 / min(gScreenSize.x, gScreenSize.y);
	gWorldUV = gl_Position.xy * mat2(gScreenSize.x * eigenv, 0, 0, gScreenSize.y * eigenv);
	gWorldUV = (gWorldUV + 1.0) * 0.5; // Unorm
}