#pragma once
#include <sGFX/Core/Types.hpp>
#include "ImageData.h"
#include <initializer_list>

namespace sGFX
{
	enum class TextureFormat
	{
		None,
		Lum_u8,
		Lum_f16,
		Lum_f32,
		RGB_u8,
		RGB_f16,
		RGB_f32,
		RGBA_u8,
		RGBA_f16,
		RGBA_f32
	};

	struct Texture
	{
		uint32_t id = 0;
		Vec2I size = 0;
		//int depth = 0; // 0 for 2D texture, >0 for 3D textures/texture arrays.

		constexpr Texture(){};
		constexpr Texture(uint32_t _id, Vec2I _size){ id = _id; size = _size; };
		static Texture Setup(TextureFormat format, int width, int height, int mip_map_levels = 8);
		static Texture Placeholder(int width, int height, int mip_map_levels = 8);
		static Texture ImportEXR(const char* path, int mip_levels = 8);
		static Texture ImportEXRArray(const std::initializer_list<const char*>& array, int mip_levels = 8);

		// NOTE: This is more expensive than calling Framebuffer::clear
		void clear(Vec4F color);
		void generate_mip_maps();
		void set_filtering(bool enable);
		ImageData<Vec4F> download(int mip_level = 0);

		operator bool(){ return id; };
		bool operator==(Texture other){ return id == other.id; };
	};
}