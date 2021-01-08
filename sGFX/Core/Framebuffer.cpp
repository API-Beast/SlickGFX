#include "GL/flextGL.h"
#include "Framebuffer.hpp"

void sGFX::Framebuffer::reset_size(int width, int height) 
{
	
}

void sGFX::Framebuffer::copy_to_screen() 
{
	// 0 is default framebuffer, e.g. the screen.
	glBlitNamedFramebuffer(id, 0, 0, 0, size[0], size[1], 0, 0, size[0], size[1], GL_COLOR_BUFFER_BIT, GL_NEAREST);
}