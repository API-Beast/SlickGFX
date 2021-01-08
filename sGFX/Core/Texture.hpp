#pragma once
#include "Types.hpp"

namespace sGFX
{
	struct Texture
	{
		int id = 0;

		Vec2I size = {0, 0};

		void generate_mip_maps();
		void reset_size(int width, int height);
		void clear(Vec4F color);
	};
}