#pragma once

#include <sGFX/Core/Types.hpp>
#include "Texture.hpp"
#include <vector>

namespace sGFX
{

struct [[slick::tuple]] RenderAttachment
{
	TextureFormat format;
	int mip_levels = 1;
};

struct [[slick::object]] FramebufferSpec
{
	RenderAttachment depth = {TextureFormat::None};
	RenderAttachment stencil = {TextureFormat::None};

	std::vector<RenderAttachment> colors;
};

struct Framebuffer
{
	unsigned int id = 0;
	std::vector<Texture> color;
	Texture depth;
	Texture stencil;
	Vec2I size = 0;
	FramebufferSpec spec;

	void recreate(int w, int h);
	void create_from_spec(int width, int height, const FramebufferSpec& spec);
	void clear_attachments(Vec4F color = Vec4F(0.0), float depth = 0.0, int stencil = 0);
	void copy_color_to(Framebuffer& other, int index);
	void free_data(bool keep_textures = false);
	void set_attachment_bindings(std::vector<int> list); // For default constructed framebuffers: binding[N] = color[N]
};

}