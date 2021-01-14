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
	pass.shader.set_uniform(0, sGFX::Vec2I(width, height));
	main_loop(window);
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	GLFWwindow* window = glfwCreateWindow(800, 600, "sGFX Screenspace Example", NULL, NULL);
	glfwSwapInterval(1); // VSync
	sGFX::SetupOpenGL(window);

	std::vector<unsigned char> frag_shader = slick::import_file("screen_fragment.glsl");
	std::vector<unsigned char> vert_shader = slick::import_file("screen_vertex.glsl");
	sGFX::RenderPassSpec spec = {
		frag_shader, /* fragment shader */
		vert_shader, /* vertex shader */
		{sGFX::TextureFormat::None}, /* depth attachment */
		{sGFX::TextureFormat::None}, /* stencil attachment */
		{{sGFX::TextureFormat::RGB_f16}}, /* color attachments */

		{}, /* vertex attributes */
		{}, /* instance attributes */
		0,  /* vertex attribute buffer length */
		0   /* instance attribute buffer length */
	};
	pass.create_from_spec(spec, 800, 600);

	glfwSetWindowSizeCallback(window, resize_loop);
	while(!glfwWindowShouldClose(window))
	{
		main_loop(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}