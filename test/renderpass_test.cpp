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
		{}, {}, 0, 0, 0
	};
	RenderPass pass;
	Framebuffer fbo;
	fbo.create_from_spec(2, 2, {{TextureFormat::None}, {TextureFormat::None}, {{TextureFormat::RGBA_f16}}});
	pass.create_from_spec(&fbo, spec);
	pass.draw(ctx, 3, 1);
	auto result = fbo.color[0].download();
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
		{}, {}, 0, 0, 0
	};
	const char test_glsl[] = "const float test_val = 0.5;\n";
	ctx.prepare_shader_include_data("/test.glsl", test_glsl, sizeof(test_glsl));
	RenderPass pass;
	Framebuffer fbo;
	fbo.create_from_spec(2, 2, {{TextureFormat::None}, {TextureFormat::None}, {{TextureFormat::RGBA_f16}}});
	pass.create_from_spec(&fbo, spec);
	pass.draw(ctx, 3, 1);
	EXPECT_EQ(ctx.has_error(), false);
}

YES_TEST(RenderPass, bind_shader_buffers)
{
	RenderAPIContext ctx = RenderAPIContext::SetupOffscreen();
	RenderPassSpec spec = {
		R"(#version 450 core
		layout(std140, binding = 13) uniform data
		{
			float test_val;
		};
		layout(std430, binding = 13) buffer storage
		{
			float test_val_2;
		};
		layout(location = 0) out vec4 gColor;
		void main(){ gColor = vec4(test_val + test_val_2 / 10.0); };)",
		R"(#version 450 core
		void main(){ vec2 snorm_uv = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2);
		gl_Position = vec4(snorm_uv * 2.0f + -1.0f, 0.0f, 1.0f);})",
		{}, {}, 0, 0, 0
	};
	struct 
	{
		float test_val = 0.75f;
	} uniform_data;
	struct 
	{
		float test_val = 10.0f;
	} storage_data;

	AttributeBuffer buf = AttributeBuffer::SetupUploadBuffer(sizeof(uniform_data));
	AttributeBuffer other_buf = AttributeBuffer::SetupUploadBuffer(sizeof(uniform_data));
	buf.upload_data((uint8_t*)&uniform_data, sizeof(uniform_data));
	other_buf.upload_data((uint8_t*)&storage_data, sizeof(storage_data));
	ctx.bind_uniform_buffer(13, buf);
	ctx.bind_shader_storage(13, other_buf);

	RenderPass pass;
	Framebuffer fbo;
	fbo.create_from_spec(2, 2, {{TextureFormat::None}, {TextureFormat::None}, {{TextureFormat::RGBA_f16}}});
	pass.create_from_spec(&fbo, spec);
	pass.draw(ctx, 3, 1);

	auto result = fbo.color[0].download();
	Vec4F val = uniform_data.test_val + storage_data.test_val / 10.0f;
	std::vector<Vec4F> expected = {val, val, val ,val};
	EXPECT_CONTAINER_EQ(result, expected);
}