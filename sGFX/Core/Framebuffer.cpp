#include "GL/flextGL.h"
#include "Framebuffer.hpp"

namespace sGFX
{

void Framebuffer::free_data(bool keep_textures)
{
	if(id) glDeleteFramebuffers(1, &id);
	if(!keep_textures)
	{
		for(Texture& color_tex : color) glDeleteTextures(1, &color_tex.id);
		if(depth)   glDeleteTextures(1, &depth.id);
		if(stencil) glDeleteTextures(1, &stencil.id);

		color.clear();
		depth.id = 0;
		stencil.id = 0;
	}

	id = 0;
}

void Framebuffer::clear_attachments(Vec4F _color, float _depth, int _stencil)
{
	static int prev_stencil = 0;
	static float prev_depth = 0.0;
	static Vec4F prev_color = Vec4F(0.0);

	int prev_framebuffer = 0;
	glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &prev_framebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, id);

	if(prev_color != _color) glClearColor(_color[0], _color[1], _color[2], _color[3]);
	if(prev_depth != _depth) glClearDepthf(_depth);
	if(prev_stencil != _stencil) glClearStencil(_stencil);

	glClear(GL_COLOR | GL_DEPTH | GL_STENCIL);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, prev_framebuffer);

	prev_color = _color;
	prev_depth = _depth;
	prev_stencil = _stencil;
}

void Framebuffer::copy_to_screen() 
{
	// 0 is default framebuffer, e.g. the backbuffer of the screen.
	glBlitNamedFramebuffer(id, 0, 0, 0, size[0], size[1], 0, 0, size[0], size[1], GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

}