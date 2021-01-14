#include "Texture.hpp"
#include "GL/flextGL.h"

namespace sGFX
{
	void Texture::generate_mip_maps() 
	{
		glGenerateTextureMipmap(id);
	}
	
	void Texture::clear(Vec4F color) 
	{
		glClearTexImage(id, 0, GL_RGBA, GL_FLOAT, (float*)&color);
	}
}