#include "Texture.hpp"
#include "GL/flextGL.h"

namespace sGFX
{
	Texture Texture::Setup(TextureFormat f, int width, int height, int mip_levels) 
	{
		GLuint texture;
		GLenum format = 0;
		switch(f)
		{
			case TextureFormat::None:
				break;
			case TextureFormat::Lum_u8:
				format = GL_R8UI;
				break;
			case TextureFormat::Lum_f16:
				format = GL_R16F;
				break;
			case TextureFormat::Lum_f32:
				format = GL_R32F;
				break;
			case TextureFormat::RGB_u8:
				format = GL_RGB8UI;
				break;
			case TextureFormat::RGB_f16:
				format = GL_RGB16F;
				break;
			case TextureFormat::RGB_f32:
				format = GL_RGB32F;
				break;
			case TextureFormat::RGBA_u8:
				format = GL_RGBA8UI;
				break;
			case TextureFormat::RGBA_f16:
				format = GL_RGBA16F;
				break;
			case TextureFormat::RGBA_f32:
				format = GL_RGBA32F;
				break;
		}
		if(format == 0)
			return {};
		glCreateTextures(GL_TEXTURE_2D, 1, &texture);
		glTextureStorage2D(texture, mip_levels, format, width, height);
		return {texture, Vec2I(width, height)};
	}

	void Texture::generate_mip_maps() 
	{
		glGenerateTextureMipmap(id);
	}
	
	void Texture::set_filtering(bool enable) 
	{
		if(enable)
		{
			glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else
		{
			glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
			glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
	}
	
	ImageData<Vec4F> Texture::download(int mip_level) 
	{
		ImageData<Vec4F> retVal;
		retVal.allocate(size);
		glGetTextureImage(id, mip_level, GL_RGBA, GL_FLOAT, retVal.data_length*sizeof(Vec4F), (void*)retVal.data);
		return retVal;
	}
	
	void Texture::clear(Vec4F color) 
	{
		glClearTexImage(id, 0, GL_RGBA, GL_FLOAT, (float*)&color);
	}
}