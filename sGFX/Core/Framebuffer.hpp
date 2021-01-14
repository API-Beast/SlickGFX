#pragma once

#include "Types.hpp"
#include "Texture.hpp"
#include <vector>

namespace sGFX
{

struct Framebuffer
{
	unsigned int id = 0;
	std::vector<Texture> color;
	Texture depth;
	Texture stencil;
	Vec2I size = 0;

	void clear_attachments(Vec4F color = Vec4F(0.0), float depth = 0.0, int stencil = 0);
	void copy_to_screen();
	void free_data(bool keep_textures = false);
};

}