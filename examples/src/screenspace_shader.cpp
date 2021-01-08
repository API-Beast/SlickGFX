#include <sGFX/Core.hpp>
#include <sGFX/Core/GL/flextGL.h>
#include <slick/slick.hpp>
#include <glfw/glfw3.h>

int main()
{
	using namespace sGFX;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
	GLFWwindow* window = glfwCreateWindow(800, 600, "sGFX Screenspace Example", NULL, NULL);
	glfwSwapInterval(1); // VSync
	sGFX::SetupOpenGL(window);

	std::vector<unsigned char> frag_shader = slick::import_file("screen_fragment.glsl");
	std::vector<unsigned char> vert_shader = slick::import_file("screen_vertex.glsl");
	RenderPassSpec spec = {
		frag_shader, /* fragment shader */
		vert_shader, /* vertex shader */
		{TextureFormat::None}, /* depth attachment */
		{TextureFormat::None}, /* stencil attachment */
		{{TextureFormat::RGB_f16}}, /* color attachments */

		{}, /* vertex attributes */
		{}, /* instance attributes */
		0,  /* vertex attribute buffer length */
		0   /* instance attribute buffer length */
	};
	RenderPass pass(spec, 800, 600);
	/* Usually we would push data to the vertex and instance buffers here. */
	/* However, this shader has no attributes so we just omit this step. */
	while(!glfwWindowShouldClose(window))
	{
		pass.fbo.color[0].clear(0.0);
		pass.draw(3, 1); // 4 vertices, 1 instance
		pass.fbo.copy_to_screen();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}