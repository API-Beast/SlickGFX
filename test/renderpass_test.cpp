#include "test_utils.h"
#include "yestest.h"

#include <sGFX/Core.hpp>

using namespace sGFX;

YES_TEST(RenderPass, screenspace)
{
	RenderAPIContext ctx = RenderAPIContext::SetupOffscreen();
	RenderPassSpec spec = {
		R"(#version 450 core
		layout(location = 0) out vec4 gColor;
		void main(){ gColor = vec4(0.5); };)",
		R"(#version 450 core
		void main(){ vec2 snorm_uv = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2);
		gl_Position = vec4(snorm_uv * 2.0f + -1.0f, 0.0f, 1.0f);})",
		{TextureFormat::None}, {TextureFormat::None}, {{TextureFormat::RGBA_f16}}, {}, {}, 0, 0, 0
	};
	RenderPass pass;
	pass.create_from_spec(spec, 2, 2);
	pass.draw(3, 1);
	auto result = pass.fbo.color[0].download();
	std::vector<Vec4F> expected = {0.5f, 0.5f, 0.5f, 0.5f};
	EXPECT_CONTAINER_EQ(result, expected);
}

YES_TEST(RenderPass, include)
{
	RenderAPIContext ctx = RenderAPIContext::SetupOffscreen();
	RenderPassSpec spec = {
		R"(#version 450 core
		#extension GL_ARB_shading_language_include : require
		#include </test.glsl>
		layout(location = 0) out vec4 gColor;
		void main(){ gColor = vec4(test_val); };)",
		R"(#version 450 core
		void main(){ vec2 snorm_uv = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2);
		gl_Position = vec4(snorm_uv * 2.0f + -1.0f, 0.0f, 1.0f);})",
		{TextureFormat::None}, {TextureFormat::None}, {{TextureFormat::RGBA_f16}}, {}, {}, 0, 0, 0
	};
	const char test_glsl[] = "const float test_val = 0.5;\n";
	ctx.prepare_shader_include_data("/test.glsl", test_glsl, sizeof(test_glsl));
	RenderPass pass;
	pass.create_from_spec(spec, 2, 2);
	pass.draw(3, 1);
	EXPECT_EQ(ctx.has_error(), false);
}