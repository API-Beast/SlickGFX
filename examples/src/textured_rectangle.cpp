#include <sGFX/Core.hpp>
#include <sGFX/Core/GL/flextGL.h>
#include <slick/slick.hpp>
#include <glfw/glfw3.h>

sGFX::RenderPass pass;

void main_loop(GLFWwindow* window)
{
	pass.fbo.color[0].clear(0.0);
	pass.draw(3, 1); // 3 vertices, 1 instance
	pass.fbo.copy_to_screen();
	glfwSwapBuffers(window);
}

void resize_loop(GLFWwindow* window, int width, int height)
{
	pass.recreate_framebuffer(width, height);
	pass.shader.set_uniform(0, sGFX::Vec2F(width, height));
	main_loop(window);
}

int main()
{
	sGFX::RenderAPIContext ctx = sGFX::RenderAPIContext::SetupWindowGLFW("sGFX Texture Test", 800, 600, sGFX::RenderAPIContext::Resizable | sGFX::RenderAPIContext::VSync);
	GLFWwindow* window = ctx.get_glfw_window_handle();

	auto frag_shader = slick::import_file("screen_texture.fragment.glsl");
	auto vert_shader = slick::import_file("screen_vertex.glsl");
	sGFX::RenderPassSpec spec = {
		frag_shader, /* fragment shader */
		vert_shader, /* vertex shader */
		{sGFX::TextureFormat::None}, /* depth attachment */
		{sGFX::TextureFormat::None}, /* stencil attachment */
		{{sGFX::TextureFormat::RGB_f16}}, /* color attachments */ // TODO: Switch to uint8, currently not supported

		{}, /* vertex attributes */
		{}, /* instance attributes */
		0,  /* vertex attribute buffer length */
		0   /* instance attribute buffer length */
	};
	pass.create_from_spec(spec, 800, 600);
	pass.shader.set_uniform(0, sGFX::Vec2F(800, 600));

	sGFX::Texture image = sGFX::Texture::ImportEXR("sample.exr");
	//sGFX::Texture image = sGFX::Texture::Placeholder(512, 512);
	pass.shader.set_uniform(1, image.size.max_aspect());

	ctx.bind_texture(0, image);
	pass.shader.set_uniform(2, 0);

	glfwSetWindowSizeCallback(window, resize_loop);
	while(!glfwWindowShouldClose(window))
	{
		main_loop(window);
		glfwPollEvents();
	}
	return 0;
}