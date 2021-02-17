#include <sGFX/Core/GL/flextGL.h>
#include "Framebuffer.hpp"

namespace sGFX
{

void Framebuffer::recreate(int w, int h) 
{
	create_from_spec(spec, w, h);
}

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

void Framebuffer::create_from_spec(const FramebufferSpec& spec, int width, int height)
{
	const char context_name[] = "Framebuffer::create_from_spec()";
	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, __LINE__, sizeof(context_name), context_name);

	free_data();

	this->spec = spec;

	// Frame buffer
	GLuint fbo_name = 0;
	glCreateFramebuffers(1, &fbo_name);
	this->id = fbo_name;
	this->size = Vec2I(width, height);

	this->depth = Texture::Setup(spec.depth.format, width, height, spec.depth.mip_levels);
	if(this->depth.id) glNamedFramebufferTexture(fbo_name, GL_DEPTH_ATTACHMENT, this->depth.id, 0);

	this->stencil = Texture::Setup(spec.stencil.format, width, height, spec.stencil.mip_levels);
	if(this->stencil.id) glNamedFramebufferTexture(fbo_name, GL_STENCIL_ATTACHMENT, this->stencil.id, 0);
	
	GLenum slot = GL_COLOR_ATTACHMENT0;
	for(const RenderAttachment& r : spec.colors)
	{
		Texture t = Texture::Setup(r.format, width, height, r.mip_levels);
		glNamedFramebufferTexture(fbo_name, slot, t.id, 0);
		this->color.push_back(t);
		slot += 1;
	}

	std::vector<GLenum> buffers(spec.colors.size());
	for(int i = 0; i<buffers.size(); i++)
		buffers[i] = GL_COLOR_ATTACHMENT0 + i;
	glNamedFramebufferDrawBuffers(fbo_name, buffers.size(), buffers.data());

	glPopDebugGroup();
}

void Framebuffer::set_attachment_bindings(std::vector<int> list) 
{
	GLenum* entries = new GLenum[list.size()];
	for(int i = 0; i<list.size(); i++)
	{
		if(list[i] == -1) entries[i] = GL_NONE;
		if(id != 0)
			entries[i] = GL_COLOR_ATTACHMENT0 + list[i];
		else
			entries[i] = GL_FRONT_LEFT + list[i];
	}
	glNamedFramebufferDrawBuffers(id, list.size(), entries);
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

void Framebuffer::copy_color_to(Framebuffer& other, int index)
{
	glNamedFramebufferReadBuffer(id, GL_COLOR_ATTACHMENT0 + index);
	glBlitNamedFramebuffer(id, other.id, 0, 0, size[0], size[1], 0, 0, other.size[0], other.size[1], GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

}