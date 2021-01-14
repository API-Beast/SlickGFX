
#include "GL.hpp"
#include "GL/flextGL.h"

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

}

namespace sGFX
{
	void SetupOpenGL(GLFWwindow* window)
	{
		glfwMakeContextCurrent(window);
		flextInit(window);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(DebugOpenGL, NULL);
		//glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true);
	}
}