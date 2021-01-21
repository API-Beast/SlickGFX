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
	Vec4F expected_value = 0.5f;
	std::vector<Vec4F> expected = {0.5f, 0.5f, 0.5f, 0.5f};
	EXPECT_CONTAINER_EQ(result, expected);
}