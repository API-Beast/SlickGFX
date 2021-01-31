#include "Texture.hpp"
#include <sGFX/Core/GL/flextGL.h>
#include "ImageFormats/tinyexr.h"
#include <cstring>
#include <cstdio>
#include <vector>

namespace
{
	struct EXRImageLoader
	{
		int width, height;
		int texture_format = 0;
		int upload_format = 0;
		int type = 0;
		int num_channels = 0;
		EXRImage image;

		bool load_from_disk(const char* path)
		{
			EXRVersion version;
			EXRHeader  header;
			const char* error = nullptr;
			auto print_and_clear = [error, path](){
				std::fprintf(stderr, "Error parsing %s EXRImageLoader: %s\n", path, error);
				FreeEXRErrorMessage(error);
				glPopDebugGroup();
			};

			if(ParseEXRVersionFromFile(&version, path) != 0){ std::printf("%s is not an EXRImageLoader file.\n", path); return false; }
			if(version.multipart){ std::printf("Multipart EXRs are not supported."); return false; }
			
			InitEXRHeader(&header);

			if(ParseEXRHeaderFromFile(&header, &version, path, &error) != 0){ print_and_clear(); return false; }

			num_channels = header.num_channels;
			if(num_channels > 4) num_channels = 4;
			if(num_channels == 1) upload_format = GL_RED;
			if(num_channels == 2) upload_format = GL_RG;
			if(num_channels == 3) upload_format = GL_RGB;
			if(num_channels == 4) upload_format = GL_RGBA;

			if(header.pixel_types[0] == TINYEXR_PIXELTYPE_HALF)
			{
				type = GL_FLOAT;
				if(num_channels == 1) texture_format = GL_R16F;
				if(num_channels == 2) texture_format = GL_RG16F;
				if(num_channels == 3) texture_format = GL_RGB16F;
				if(num_channels == 4) texture_format = GL_RGBA16F;
			}
			else if(header.pixel_types[0] == TINYEXR_PIXELTYPE_FLOAT)
			{
				type = GL_FLOAT;
				if(num_channels == 1) texture_format = GL_R32F;
				if(num_channels == 2) texture_format = GL_RG32F;
				if(num_channels == 3) texture_format = GL_RGB32F;
				if(num_channels == 4) texture_format = GL_RGBA32F;
			}
			else
			{
				type = GL_UNSIGNED_INT;
				if(num_channels == 1) texture_format = GL_R32UI;
				if(num_channels == 2) texture_format = GL_RG32UI;
				if(num_channels == 3) texture_format = GL_RGB32UI;
				if(num_channels == 4) texture_format = GL_RGBA32UI;
			}

			for(int i = 0; i < header.num_channels; i++)
				if(header.requested_pixel_types[i] == TINYEXR_PIXELTYPE_HALF)
					header.requested_pixel_types[i] = TINYEXR_PIXELTYPE_FLOAT;
			
			InitEXRImage(&image);
			if(LoadEXRImageFromFile(&image, &header, path, &error) != 0){ print_and_clear(); return false; }

			width = image.width;
			height = image.height;

			FreeEXRHeader(&header);
			return true;
		}

		bool upload_to_texture(int texture, int layer = -1)
		{
			EXRTile  imageTile;
			EXRTile* tiles = image.tiles;
			int num_tiles = image.num_tiles;
			if(tiles == nullptr)
			{
				imageTile.images = image.images;
				imageTile.width = image.width;
				imageTile.height = image.height;
				imageTile.offset_x = 0;
				imageTile.offset_y = 0;
				num_tiles = 1;
				tiles = &imageTile;
			}

			int element_size = 1;
			if(type == GL_FLOAT) element_size = sizeof(float);
			if(type == GL_UNSIGNED_INT) element_size = sizeof(GLuint);

			int num_bytes = tiles[0].width * tiles[0].height * element_size;
			uint8_t* pixels = new uint8_t[num_bytes * num_channels];
			int channel_mapping[4] = {3, 2, 1, 0};

			for(int i = 0; i < num_tiles; i++)
			{
				for(int j = 0; j < num_channels; j++)
				for(int y = 0; y < tiles[i].height; y++)
				for(int x = 0; x < tiles[i].width; x++)
				{
					int offset = (x + y * tiles[i].width) * element_size;
					std::memcpy(pixels + (offset * num_channels) + channel_mapping[j] * element_size, tiles[i].images[j] + offset, element_size);
				}
				if(layer == -1)
					glTextureSubImage2D(texture, 0, tiles[i].offset_x, tiles[i].offset_y, tiles[i].width, tiles[i].height, upload_format, type, pixels);
				else
					glTextureSubImage3D(texture, 0, tiles[i].offset_x, tiles[i].offset_y, layer, tiles[i].width, tiles[i].height, 1, upload_format, type, pixels);
			}
			
			delete[] pixels;
			FreeEXRImage(&image);
			return true;
		}
	};

}

namespace sGFX
{
	Texture Texture::Setup(TextureFormat f, int width, int height, int mip_levels) 
	{
		GLuint texture;
		GLenum texture_format = 0;
		switch(f)
		{
			case TextureFormat::None:
				break;
			case TextureFormat::Lum_u8:
				texture_format = GL_R8UI;
				break;
			case TextureFormat::Lum_f16:
				texture_format = GL_R16F;
				break;
			case TextureFormat::Lum_f32:
				texture_format = GL_R32F;
				break;
			case TextureFormat::RGB_u8:
				texture_format = GL_RGB8UI;
				break;
			case TextureFormat::RGB_f16:
				texture_format = GL_RGB16F;
				break;
			case TextureFormat::RGB_f32:
				texture_format = GL_RGB32F;
				break;
			case TextureFormat::RGBA_u8:
				texture_format = GL_RGBA8UI;
				break;
			case TextureFormat::RGBA_f16:
				texture_format = GL_RGBA16F;
				break;
			case TextureFormat::RGBA_f32:
				texture_format = GL_RGBA32F;
				break;
		}
		if(texture_format == 0)
			return {};
		glCreateTextures(GL_TEXTURE_2D, 1, &texture);
		glTextureStorage2D(texture, mip_levels, texture_format, width, height);
		return {texture, Vec2I(width, height)};
	}
	
	Texture Texture::Placeholder(int width, int height, int mip_map_levels) 
	{
		Texture retVal = Setup(TextureFormat::Lum_f16, width, height);
		float checkerboard_black[256];
		std::fill(checkerboard_black, &checkerboard_black[256], 0.25);
		float checkerboard_white[256];
		std::fill(checkerboard_white, &checkerboard_white[256], 0.75);
		for(int x = 0; x <= ((width-1)/16); x++)
		for(int y = 0; y <= ((height-1)/16); y++)
		{
			if((x + y) % 2)
				glTextureSubImage2D(retVal.id, 0, x*16, y*16, 16, 16, GL_RED, GL_FLOAT, checkerboard_black);
			else
				glTextureSubImage2D(retVal.id, 0, x*16, y*16, 16, 16, GL_RED, GL_FLOAT, checkerboard_white);
		}
		if(mip_map_levels > 1)
			glGenerateTextureMipmap(retVal.id);
		return retVal;
	}
	
	Texture Texture::ImportEXR(const char* path, int mip_levels) 
	{
		EXRImageLoader exr;
		if(!exr.load_from_disk(path)) return {};

		const char context_name[] = "Texture::ImportEXR";
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, __LINE__, sizeof(context_name), context_name);

		GLuint texture;
		glCreateTextures(GL_TEXTURE_2D, 1, &texture);
		glTextureStorage2D(texture, mip_levels, exr.texture_format, exr.width, exr.height);
		exr.upload_to_texture(texture);
		if(mip_levels > 1)
			glGenerateTextureMipmap(texture);

		glPopDebugGroup();
		return {texture, Vec2I(exr.width, exr.height)};
	}
	
	Texture Texture::ImportEXRArray(const std::initializer_list<const char*>& array, int mip_levels) 
	{
		const char context_name[] = "Texture::ImportEXRArray";
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, __LINE__, sizeof(context_name), context_name);

		Vec2I tex_size = 0;
		std::vector<EXRImageLoader> imgs;
		imgs.reserve(array.size());
		assert(array.size() > 0);

		for(const char* path : array)
		{
			imgs.emplace_back();
			EXRImageLoader& exr = imgs.back();
			exr.load_from_disk(path);
			if(exr.width  > tex_size[0]) tex_size[0] = exr.width;
			if(exr.height > tex_size[1]) tex_size[1] = exr.height;
		}

		GLuint texture;
		glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &texture);
		glTextureStorage3D(texture, mip_levels, imgs[0].texture_format, tex_size[0], tex_size[1], array.size());

		int layer = 0;
		for(EXRImageLoader& exr : imgs)
			exr.upload_to_texture(texture, layer++);

		glPopDebugGroup();
		return {texture, tex_size};
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