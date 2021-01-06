#version 460 core
precision mediump float​;

vec3 snorm_to_unorm(vec3 val)
{
	return (val + 1.0) / 2.0;
}

#ifdef VERTEX_SHADER
// When compiling as vertex shader, VERTEX_MAIN will be defined as main
void VERTEX_MAIN()
{
	// Since we always want a full screen we don't need to set up any vertex attributes.
	const vec4 vertices[] = vec4[](vec4(-1.0, -1.0, 0.0, 0.0),
	                               vec4(-1.0, +1.0, 0.0, 0.0),
	                               vec4(+1.0, -1.0, 0.0, 0.0),
	                               vec4(+1.0, +1.0, 0.0, 0.0));
	gl_Position = vertices[gl_VertexID];  
}
#endif

#ifdef FRAGMENT_SHADER
out vec4 gColor;

// When compiling as fragment shader, FRAGMENT_MAIN will be defined as main
void FRAGMENT_MAIN()
{
	gColor.rgb = snorm_to_unorm(gl_FragCoord.rgb);
	gColor.a = 1.0;
}
#endif