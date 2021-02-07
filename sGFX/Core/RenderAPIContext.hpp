#pragma once

#include <sGFX/Core/Types.hpp>
#include <sGFX/Core/AttributeBuffer.hpp>
#include <sGFX/Core/Texture.hpp>
#include <sGFX/Core/Framebuffer.hpp>
#include <sGFX/Resources/Signal.hpp>

struct GLFWwindow;

namespace sGFX
{
	struct RenderAPIContextData;
	struct ShaderProgram;

	struct WindowState
	{
		Vec2I offset;
		Vec2I size;
		int screen;
		bool maximized;
		bool minimized;
	};

	// FIXME This is all quick and dirty right now to ease the transition into something more useful later.
	// The very fundamental problem is the OpenGL is not designed to be object oriented, has a lot of global state etc.
	// This class will contain a copy of the state in the future in order to be able to do graceful state.
	struct RenderAPIContext
	{
		RenderAPIContext(void* win_ref);
		RenderAPIContext();
		RenderAPIContext(RenderAPIContext& other) = delete;
		RenderAPIContext(RenderAPIContext&& other);
		~RenderAPIContext();

		RenderAPIContext& operator=(RenderAPIContext&& other);

		enum Flags
		{
			None = 0,
			Hidden = 1,
			Resizable = 2,
			VSync = 4,
			ReleaseContext = 8
		};

		static RenderAPIContext SetupWindowGLFW(const char* name, int width, int height, int flags = VSync, RenderAPIContext* share_resources = nullptr);
		static RenderAPIContext SetupOffscreen(int flags = None, RenderAPIContext* share_resources = nullptr);

		void make_active();

		void prepare_shader_include(const char* path, const char* bind_to = nullptr);
		void prepare_shader_include_data(const char* path, const char* data, int data_length);

		void bind_shader_storage(int idx, const AttributeBuffer& b, bool whole_buffer = false);
		void bind_uniform_buffer(int idx, const AttributeBuffer& b, bool whole_buffer = false);
		void bind_texture(int idx, const Texture& t);

		void bind_framebuffer(const Framebuffer& fbo);
		void bind_shader_program(const ShaderProgram& prog);
		void bind_vertex_array(uint32_t vertex_array);
		void bind_transform_feedback_buffer(uint32_t transform_feedback_buffer);

		Framebuffer& screen_buffer();

		bool has_error();

		bool termination_requested();
		void request_termination(bool request=true);
		
		void swap_buffers();
		void poll_events();

		GLFWwindow* get_glfw_window_handle();

		RenderAPIContextData* d = nullptr;
		
		Signal<int, int> on_screen_buffer_resize;
	};
}