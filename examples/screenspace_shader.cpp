#include <sGFX/Core/RenderPass.h>
#include <sGFX/Core/FrameBuffer.h>

void main()
{
	using namespace sGFX;

	glfwInit();
	// We need at least a 4.5 context
	// TODO: Implement context version checks to sGFX and warn if version is too old for a specific feature.
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(800, 600, "sGFX Screenspace Example", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // VSync
	sGFX::GL::set_gl_function_loader(glfwGetProcAddress);
	sGFX::GL::load_all_functions();

	FrameBuffer fbo;
	fbo.bind_color_output(0, RGBA_f16);

	RenderPass screenspacePass(fbo, "screenspace.glsl");
	//screenspacePass.bind_vertex_attribute<VertexData>(&VertexData::fooBar, "fooBar");

	while(!glfwWindowShouldClose(window))
	{
		screenspacePass.draw();


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}