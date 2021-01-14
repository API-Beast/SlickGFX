#pragma once
#include "Types.hpp"

namespace sGFX
{
	struct Texture
	{
		uint32_t id = 0;

		// NOTE: This is more expensive than calling Framebuffer::clear
		void clear(Vec4F color);
		void generate_mip_maps();

		operator bool(){ return id; };
		bool operator==(Texture other){ return id == other.id; };
	};
}