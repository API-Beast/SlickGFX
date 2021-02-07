#include <sGFX/Core.hpp>
#include <sGFX/Core/GL/flextGL.h>
#include <slick/slick.hpp>
#include <glfw/glfw3.h>

sGFX::RenderPass pass;
sGFX::Framebuffer fbo;
sGFX::RenderAPIContext ctx;

void main_loop()
{
	fbo.color[0].clear(0.0);
	pass.draw(ctx, 3, 1); // 3 vertices, 1 instance
	fbo.copy_color_to(ctx.screen_buffer());
	ctx.swap_buffers();
}

void resize_loop(int width, int height)
{
	fbo.recreate(width, height);
	pass.shader.set_uniform(0, sGFX::Vec2F(width, height));
	main_loop();
}

int main()
{
	ctx = sGFX::RenderAPIContext::SetupWindowGLFW("sGFX Screenspace Test", 800, 600, sGFX::RenderAPIContext::Resizable | sGFX::RenderAPIContext::VSync);

	auto frag_shader = slick::import_file("screen_texture.fragment.glsl");
	auto vert_shader = slick::import_file("screen_vertex.glsl");
	sGFX::FramebufferSpec fb_spec =
	{
		{sGFX::TextureFormat::None}, /* depth attachment */
		{sGFX::TextureFormat::None}, /* stencil attachment */
		{{sGFX::TextureFormat::RGB_f16}} /* color attachments */
	};
	sGFX::RenderPassSpec spec = {
		frag_shader, /* fragment shader */
		vert_shader, /* vertex shader */

		{}, /* vertex attributes */
		{}, /* instance attributes */
		0,  /* vertex attribute buffer length */
		0   /* instance attribute buffer length */
	};
	fbo.create_from_spec(800, 600, fb_spec);
	pass.create_from_spec(&fbo, spec);
	pass.shader.set_uniform(0, sGFX::Vec2F(800, 600));

	sGFX::Texture image = sGFX::Texture::ImportEXR("sample.exr");
	pass.shader.set_uniform(1, image.size.max_aspect());

	ctx.bind_texture(0, image);
	pass.shader.set_uniform(2, 0);

	ctx.on_screen_buffer_resize.connect(resize_loop);
	while(!ctx.termination_requested())
	{
		main_loop();
		ctx.poll_events();
	}
	return 0;
}