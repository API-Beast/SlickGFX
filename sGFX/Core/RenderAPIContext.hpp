#pragma once

#include <sGFX/Core/Types.hpp>
#include <sGFX/Core/AttributeBuffer.hpp>
#include <sGFX/Core/Texture.hpp>

struct GLFWwindow;

namespace sGFX
{
	struct RenderAPIContextData;

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
		~RenderAPIContext();

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

		bool has_error();
		
		GLFWwindow* get_glfw_window_handle();

		RenderAPIContextData* d = nullptr;
	};
}