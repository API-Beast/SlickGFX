#include <sGFX/Core/GL/flextGL.h>
#include "RenderPass.hpp"

#include <cstdio>


namespace sGFX
{
	RenderPass::RenderPass(const RenderPassSpec& spec, int width, int height) 
	{
		const char context_name[] = "RenderPass()";
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 9089, sizeof(context_name), context_name);

		char* sources_frag[] = {(char*)spec.shader_fragment.data()};
		char* sources_vert[] = {(char*)spec.shader_vertex.data()};
		int frag_sources_lengths[] = {spec.shader_fragment.size()};
		int vert_sources_lengths[] = {spec.shader_vertex.size()};

		GLuint program = glCreateProgram();
		GLuint frag_shader = glCreateShader(GL_VERTEX_SHADER);
		GLuint vert_shader = glCreateShader(GL_FRAGMENT_SHADER);
		GLsizei infoLogSize = 0;
		static char infoLog[1024 * 24];

		//glShaderSource(frag_shader, 2, sources_frag, frag_sources_lengths);
		glShaderSource(frag_shader, 1, sources_frag, frag_sources_lengths);
		glCompileShader(frag_shader);
		glAttachShader(program, frag_shader);
		glGetShaderiv(frag_shader, GL_INFO_LOG_LENGTH, &infoLogSize);
		if(infoLogSize > 0)
		{
			glGetShaderInfoLog(frag_shader, sizeof(infoLog), &infoLogSize, infoLog);
			glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH, infoLogSize, infoLog);
		}

		//glShaderSource(vert_shader, 2, sources_vert, vert_sources_lengths);
		glShaderSource(vert_shader, 1, sources_vert, vert_sources_lengths);
		glCompileShader(vert_shader);
		glAttachShader(program, vert_shader);
		glGetShaderiv(vert_shader, GL_INFO_LOG_LENGTH, &infoLogSize);
		if(infoLogSize > 0)
		{
			glGetShaderInfoLog(vert_shader, sizeof(infoLog), &infoLogSize, infoLog);
			glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH, infoLogSize, infoLog);
		}

		glLinkProgram(program);
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogSize);
		if(infoLogSize > 0)
		{
			glGetProgramInfoLog(program, sizeof(infoLog), &infoLogSize, infoLog);
			glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH, infoLogSize, infoLog);
		}

		// Frame buffer
		GLuint fbo_name = 0;
		glCreateFramebuffers(1, &fbo_name);
		fbo.id = fbo_name;
		fbo.size = Vec2I(width, height);

		int num_attachments = spec.color_attachments.size();
		GLenum slot = GL_COLOR_ATTACHMENT0;
		for(const RenderAttachment& r : spec.color_attachments)
		{
			GLuint texture;
			GLenum format = 0;
			switch(r.format)
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
				continue;
			// For simplicities sake we will use glTexImage2D instead of glTexStorage2D for now, because it allows changing the format which will come in handy for resizing the framebuffer.
			glCreateTextures(GL_TEXTURE_2D, 1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGBA,  GL_UNSIGNED_BYTE, nullptr);
			glNamedFramebufferTexture(fbo_name, slot, texture, 0);
			fbo.color.push_back({texture});
			slot += 1;
		}
		std::vector<GLenum> buffers(spec.color_attachments.size());
		for(int i = 0; i<buffers.size(); i++)
			buffers[i] = GL_COLOR_ATTACHMENT0 + i;
		glNamedFramebufferDrawBuffers(fbo_name, buffers.size(), buffers.data());
		
		// Attribute buffers
		GLuint index_buff = 0;
		GLuint vert_buff  = 0;
		GLuint inst_buff  = 0;

		int max_indices = spec.max_vertices * 8;
		int sizeof_vertex = sizeof(float) * 4;
		int sizeof_instance = sizeof(float) * 4;

		index_format = DataFormat::Uint16;
		if(spec.max_vertices > 32767)
			index_format = DataFormat::Uint32;

		glCreateBuffers(1, &vert_buff);
		glNamedBufferData(vert_buff, spec.max_vertices * sizeof_vertex, NULL, GL_DYNAMIC_DRAW);

		glCreateBuffers(1, &inst_buff);
		glNamedBufferData(inst_buff, spec.max_instances * sizeof_instance, NULL, GL_DYNAMIC_DRAW);

		glCreateBuffers(1, &index_buff);
		if(index_format == DataFormat::Uint16)
			glNamedBufferData(index_buff, max_indices * sizeof(GLushort), NULL, GL_DYNAMIC_DRAW);
		else if(index_format == DataFormat::Uint32)
			glNamedBufferData(index_buff, max_indices * sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);

		shader = {program};
		indices = {index_buff};
		vertex_attributes = {vert_buff};
		instance_attributes = {inst_buff};

		glCreateVertexArrays(1, &vertex_array_id);
		glVertexArrayElementBuffer(vertex_array_id, indices.id);
		// TODO: Bind vertex attributes
		// TODO: Bind instance attributes (vertex attributes with divisor)
		glVertexAttrib4f(0, 0.0, 0.0, 0.0, 0.0);
		glVertexAttrib4f(1, 0.0, 0.0, 0.0, 0.0);

		glPopDebugGroup();
	}

	void RenderPass::draw(int num_indices, int num_instances, int index_offset, int instance_offset, int vertex_offset)
	{
		const char context_name[] = "RenderPass::draw()";
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 9241, sizeof(context_name), context_name);

		GLenum itype = GL_UNSIGNED_SHORT;
		int isize = sizeof(GLushort);
		if(index_format == DataFormat::Uint32)
		{
			itype = GL_UNSIGNED_INT;
			isize = sizeof(GLuint);
		}
		else if(index_format == DataFormat::Uint8)
		{
			itype = GL_UNSIGNED_BYTE;
			isize = sizeof(GLubyte);
		}

		static bool initialized = false;
		if(!initialized)
		{
			glBindVertexArray(vertex_array_id);
			glUseProgram(shader.id);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo.id);
			initialized = true;
		}

		glDrawArraysInstancedBaseInstance(GL_TRIANGLE_STRIP, index_offset, num_indices, 1, instance_offset);
		//glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLE_STRIP, num_indices, itype, (void*)(isize * index_offset), num_instances, vertex_offset, instance_offset);

		glPopDebugGroup();
	}
}