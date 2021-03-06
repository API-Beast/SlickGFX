
#include "RenderAPIContext.hpp"
#include <sGFX/Core/GL/flextGL.h>
#include <sGFX/Core/ShaderProgram.hpp>
#include <sGFX/Core/Framebuffer.hpp>

#include <vector>
#include <cstdio>
#include <cstring>
#include <string>

// WHY DOES IT INCLUDE OPENGL HEADERS
#define GLFW_INCLUDE_NONE
#include <glfw/glfw3.h>

#include <slick/slick.hpp>

struct sGFX::RenderAPIContextData
{
	GLFWwindow* window = nullptr;
	Vec2I size = Vec2I(0, 0);
};

namespace
{

void DebugOpenGL(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* msg, const void* userParam)
{
	static std::vector<std::string> current_context;
	static int last_printed_context_id = 0;
	static int current_context_id = 0;
	static int last_message_id = 0;
	static int message_streak = 0;

	const char* _type;
	const char* _severity;
	const char* _color = "";
	const char* _highlight = "\u001B[93m";
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

	if(last_message_id == id)
	{
		message_streak++;
		if(message_streak == 4)
			printf("... Message is repeated, ignore further. \n");
		if(message_streak >= 4)
			return;
	}

	if(current_context_id != last_printed_context_id && current_context.size() > 0)
	{
		int max_stack_depth = 4;
		if(current_context.size() < max_stack_depth)
			max_stack_depth = current_context.size();

		for(int i = 0; i < max_stack_depth; i++)
		{
			if(i == 0)
				printf("%s[OpenGL] In %s%s\n", _highlight, current_context[i].c_str(), _color_reset);
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
	fflush(stdout);
	last_message_id = id;
};

void SetupOpenGL(GLFWwindow* window)
{
	glfwMakeContextCurrent(window);
	flextInit(window);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(DebugOpenGL, NULL);
	//glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true);
};

void Callback_OnBufferResize(GLFWwindow* win, int x, int y)
{
	sGFX::RenderAPIContext* ctx = (sGFX::RenderAPIContext*)glfwGetWindowUserPointer(win);
	ctx->d->size = sGFX::Vec2I(x, y);
	ctx->on_screen_buffer_resize.call(x, y);
};

}

namespace sGFX
{
	RenderAPIContext::RenderAPIContext(void* win_ref) 
	{
		d = new RenderAPIContextData;
		d->window = (GLFWwindow*)win_ref;
		glfwSetWindowUserPointer(d->window, (void*)this);
	}
	
	RenderAPIContext::RenderAPIContext(RenderAPIContext&& other) 
	{
		d = other.d;
		other.d = nullptr;
		glfwSetWindowUserPointer(d->window, (void*)this);
	}
	
	RenderAPIContext::RenderAPIContext() 
	{
		d = nullptr;
	}
	
	RenderAPIContext::~RenderAPIContext() 
	{
		if(d && d->window)
			glfwDestroyWindow(d->window);
		delete d;
	}
	
	RenderAPIContext& RenderAPIContext::operator=(RenderAPIContext&& other) 
	{
		d = other.d;
		other.d = nullptr;
		glfwSetWindowUserPointer(d->window, (void*)this);
		return *this;
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

		RenderAPIContext retVal(window);
		retVal.d->size = Vec2I(width, height);
		glfwSetFramebufferSizeCallback(window, Callback_OnBufferResize);
		return retVal;
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

		RenderAPIContext retVal(window);
		retVal.d->size = Vec2I(1, 1);
		glfwSetFramebufferSizeCallback(window, Callback_OnBufferResize);
		return retVal;
	}
	
	void RenderAPIContext::make_active() 
	{
		glfwMakeContextCurrent(d->window);
	}
	
	void RenderAPIContext::prepare_shader_include(const char* path, const char* bind_to) 
	{
		if(bind_to == nullptr)
			bind_to = path;
		std::string target_path(bind_to);
		if(target_path.front() != '/')
			target_path.insert(0, 1, '/');
		std::string contents = slick::import_file(path);
		//std::printf("%s", target_path.c_str());
		if(glIsNamedStringARB(target_path.size(), target_path.c_str()))
			glDeleteNamedStringARB(target_path.size(), target_path.c_str());
		glNamedStringARB(GL_SHADER_INCLUDE_ARB, target_path.size(), target_path.c_str(), contents.size(), contents.c_str());
	}
	
	void RenderAPIContext::prepare_shader_include_data(const char* path, const char* data, int data_length) 
	{
		glNamedStringARB(GL_SHADER_INCLUDE_ARB, std::strlen(path), path, data_length, data);
	}
	
	void RenderAPIContext::bind_shader_storage(int id, const AttributeBuffer& b, bool whole_buffer) 
	{
		if(whole_buffer)
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, id, b.id);
		else
			glBindBufferRange(GL_SHADER_STORAGE_BUFFER, id, b.id, 0, b.write_offset);
	}
	
	void RenderAPIContext::bind_uniform_buffer(int id, const AttributeBuffer& b, bool whole_buffer) 
	{
		if(whole_buffer)
			glBindBufferBase(GL_UNIFORM_BUFFER, id, b.id);
		else
			glBindBufferRange(GL_UNIFORM_BUFFER, id, b.id, 0, b.write_offset);
	}
	
	void RenderAPIContext::bind_texture(int idx, const Texture& t) 
	{
		glActiveTexture(GL_TEXTURE0 + idx);
		glBindTextureUnit(idx, t.id);
	}
	
	void RenderAPIContext::bind_framebuffer(const Framebuffer& fbo) 
	{
		glViewport(0, 0, fbo.size[0], fbo.size[1]);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo.id);
	}
	
	void RenderAPIContext::bind_shader_program(const ShaderProgram& shader) 
	{
		glUseProgram(shader.id);
	}
	
	void RenderAPIContext::bind_vertex_array(uint32_t vertex_array) 
	{
		glBindVertexArray(vertex_array);
	}
	
	void RenderAPIContext::bind_transform_feedback_buffer(uint32_t transform_feedback_buffer) 
	{
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transform_feedback_buffer);
	}
	
	Framebuffer& RenderAPIContext::screen_buffer() 
	{
		static Framebuffer screen;
		screen.id = 0;
		screen.size = d->size;
		return screen;
	}
	
	
	bool RenderAPIContext::has_error() 
	{
		return glGetError() != GL_NO_ERROR;
	}
	
	bool RenderAPIContext::termination_requested() 
	{
		return glfwWindowShouldClose(d->window);
	}
	
	void RenderAPIContext::request_termination(bool request) 
	{
		glfwSetWindowShouldClose(d->window, request);
	}
	
	void RenderAPIContext::swap_buffers() 
	{
		glfwSwapBuffers(d->window);
	}
	
	void RenderAPIContext::poll_events() 
	{
		glfwPollEvents();
	}
	
	GLFWwindow* RenderAPIContext::get_glfw_window_handle() 
	{
		return d->window;
	}
}