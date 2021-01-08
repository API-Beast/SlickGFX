#version 450 core

layout(location = 0) out vec2 screen_uv;

void main()
{
	// This creates an oversized triangle whose visible portion spans the entire buffer
	screen_uv = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2);
	gl_Position = vec4(screen_uv * 2.0f + -1.0f, 0.0f, 1.0f);
}