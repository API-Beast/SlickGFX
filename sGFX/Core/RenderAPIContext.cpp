
#include "RenderAPIContext.hpp"
#include <sGFX/Core/GL/flextGL.h>

#include <vector>
#include <cstdio>
#include <string>
#include <glfw/glfw3.h>



namespace
{

void DebugOpenGL(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* msg, const void* userParam)
{
	static std::vector<std::string> current_context;
	static int last_printed_context_id = 0;
	static int current_context_id = 0;

	const char* _type;
	const char* _severity;
	const char* _color = "";
	const char* _color_reset = "\u001b[0m";

	if(msg[length - 1] == '\n' || msg[length - 1] == '\0')
		length -= 1;

	switch (type)
	{
		case GL_DEBUG_TYPE_PUSH_GROUP:
			current_context.emplace_back(std::string(msg, length));
			current_context_id += id;
			return;
			break;

		case GL_DEBUG_TYPE_POP_GROUP:
			current_context.pop_back();
			current_context_id -= id;
			return;
			break;

		case GL_DEBUG_TYPE_ERROR:
			_type = "error";
			break;

		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			_type = "depreceated";
			break;

		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		case GL_DEBUG_TYPE_PORTABILITY:
			_type = "warning";
			break;

		case GL_DEBUG_TYPE_MARKER:
		case GL_DEBUG_TYPE_OTHER:
		case GL_DEBUG_TYPE_PERFORMANCE:
		default:
			_type = "info";
			break;
	}

	switch (severity) {
		case GL_DEBUG_SEVERITY_HIGH:
			_severity = "◆";
			_color = "\u001b[31m";
			break;

		case GL_DEBUG_SEVERITY_MEDIUM:
			_severity = "◈";
			_color = "\u001b[35m";
			break;

		case GL_DEBUG_SEVERITY_LOW:
		case GL_DEBUG_SEVERITY_NOTIFICATION:
		default:
			_severity = "◇";
			_color = "\u001b[36m";
			break;
	}

	if(current_context_id != last_printed_context_id && current_context.size() > 0)
	{
		int max_stack_depth = 4;
		if(current_context.size() < max_stack_depth)
			max_stack_depth = current_context.size();

		for(int i = 0; i < max_stack_depth; i++)
		{
			if(i == 0)
				printf("[OpenGL] In %s\n", current_context[i].c_str());
			else
				printf("[    ->] called by %s\n", current_context[i].c_str());
		}
		last_printed_context_id = current_context_id;
	}
	if(current_context.size() == 0)
		printf("[OpenGL]");
	else
		printf("[------]");
	printf(" %s%s %s%s: %.*s\n", _color, _severity, _type, _color_reset, length, msg);
};

void SetupOpenGL(GLFWwindow* window)
{
	glfwMakeContextCurrent(window);
	flextInit(window);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(DebugOpenGL, NULL);
	//glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true);
};

}

namespace sGFX
{
	struct RenderAPIContextData
	{
		GLFWwindow* window;
	};

	RenderAPIContext::~RenderAPIContext() 
	{
		if(d && d->window)
			glfwDestroyWindow(d->window);
		delete d;
	}

	RenderAPIContext RenderAPIContext::SetupWindowGLFW(const char* name, int width, int height, int flags, RenderAPIContext* share_resources) 
	{
		GLFWwindow* window = nullptr;
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, !(flags & ReleaseContext));
		glfwWindowHint(GLFW_RESIZABLE, flags & Resizable);
		glfwWindowHint(GLFW_VISIBLE, !(flags & Hidden));
		window = glfwCreateWindow(width, height, name, NULL, share_resources ? share_resources->d->window : nullptr);
		if(flags & VSync)
			glfwSwapInterval(1);
		SetupOpenGL(window);

		return {new RenderAPIContextData{window}};
	}
	
	RenderAPIContext RenderAPIContext::SetupOffscreen(int flags, RenderAPIContext* share_resources) 
	{
		GLFWwindow* window = nullptr;
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, !(flags & ReleaseContext));
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
		window = glfwCreateWindow(1, 1, "sGFX Offscreen Context", NULL, share_resources ? share_resources->d->window : nullptr);
		SetupOpenGL(window);

		return {new RenderAPIContextData{window}};
	}
	
	void RenderAPIContext::make_active() 
	{
		glfwMakeContextCurrent(d->window);
	}
	
	GLFWwindow* RenderAPIContext::get_glfw_window_handle() 
	{
		return d->window;
	}
}