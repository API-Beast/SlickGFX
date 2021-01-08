#include "Texture.hpp"
#include "GL/flextGL.h"

namespace sGFX
{
	void Texture::generate_mip_maps() 
	{
		
	}
	
	void Texture::reset_size(int width, int height) 
	{
		
	}
	
	void Texture::clear(Vec4F color) 
	{
		glClearTexImage(id, 0, GL_RGBA, GL_FLOAT, (float*)&color);
	}
}