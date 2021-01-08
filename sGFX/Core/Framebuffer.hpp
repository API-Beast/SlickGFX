#pragma once

#include "Types.hpp"
#include "Texture.hpp"
#include <vector>

namespace sGFX
{

struct Framebuffer
{
	int id = 0;
	std::vector<Texture> color;
	Texture depth;
	Texture stencil;
	Vec2I size = {0, 0};

	void reset_size(int width, int height);
	void copy_to_screen();
};

}