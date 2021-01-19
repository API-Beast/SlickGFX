#include <sGFX/Core/GL/flextGL.h>
#include "RenderPass.hpp"

#include <cstdio>
#include <cassert>

namespace sGFX
{
	void RenderPass::create_from_spec(const RenderPassSpec& _spec, int width, int height) 
	{
		const char context_name[] = "create_from_spec()";
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, __LINE__, sizeof(context_name), context_name);

		this->spec = _spec;

		recreate_shader();
		recreate_framebuffer(width, height);
		recreate_attribute_buffers(spec.max_vertices, spec.max_indices, spec.max_instances);

		glPopDebugGroup();
	}
	
	RenderPass::~RenderPass() 
	{
		if(vertex_array_id) glDeleteVertexArrays(1, &vertex_array_id);
		if(shader) glDeleteProgram(shader.id);
		fbo.free_data();
	}

	void RenderPass::recreate_shader() 
	{
		const char context_name[] = "recreate_shader()";
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, __LINE__, sizeof(context_name), context_name);

		if(shader)
		{
			glDeleteProgram(shader.id);
			shader.id = 0;
		}

		char* sources_frag[] = {(char*)spec.shader_fragment.data()};
		char* sources_vert[] = {(char*)spec.shader_vertex.data()};
		int frag_sources_lengths[] = {spec.shader_fragment.size()};
		int vert_sources_lengths[] = {spec.shader_vertex.size()};
		//std::string sources_frag = preprocess_shader(std::string_view((char*)spec.shader_fragment.data(), spec.shader_fragment.size()));
		//std::string sources_vert = preprocess_shader(std::string_view((char*)spec.shader_vertex.data(),   spec.shader_vertex.size()));

		GLuint program = glCreateProgram();
		GLuint frag_shader = glCreateShader(GL_VERTEX_SHADER);
		GLuint vert_shader = glCreateShader(GL_FRAGMENT_SHADER);
		GLsizei infoLogSize = 0;
		static char infoLog[1024 * 24];

		glShaderSource(frag_shader, 1, sources_frag, frag_sources_lengths);
		glCompileShader(frag_shader);
		glAttachShader(program, frag_shader);
		glGetShaderiv(frag_shader, GL_INFO_LOG_LENGTH, &infoLogSize);
		if(infoLogSize > 0)
		{
			glGetShaderInfoLog(frag_shader, sizeof(infoLog), &infoLogSize, infoLog);
			glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH, infoLogSize, infoLog);
		}

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

		// Flag for deletion, will only happen if the program actually get deleted
		glDeleteShader(frag_shader);
		glDeleteShader(vert_shader);

		shader = {program};

		glPopDebugGroup();
	}

	void RenderPass::recreate_framebuffer(int width, int height)
	{
		const char context_name[] = "recreate_framebuffer()";
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, __LINE__, sizeof(context_name), context_name);

		fbo.free_data();

		// Frame buffer
		GLuint fbo_name = 0;
		glCreateFramebuffers(1, &fbo_name);
		fbo.id = fbo_name;
		fbo.size = Vec2I(width, height);

		int num_attachments = spec.color_attachments.size();
		GLenum slot = GL_COLOR_ATTACHMENT0;
		for(const RenderAttachment& r : spec.color_attachments)
		{
			Texture t = Texture::Setup(r.format, width, height, r.mip_levels);
			glNamedFramebufferTexture(fbo_name, slot, t.id, 0);
			fbo.color.push_back(t);
			slot += 1;
		}

		fbo.depth = Texture::Setup(spec.depth_attachment.format, width, height, spec.depth_attachment.mip_levels);
		if(fbo.depth.id) glNamedFramebufferTexture(fbo_name, GL_DEPTH_ATTACHMENT, fbo.depth.id, 0);

		fbo.stencil = Texture::Setup(spec.stencil_attachment.format, width, height, spec.stencil_attachment.mip_levels);
		if(fbo.stencil.id) glNamedFramebufferTexture(fbo_name, GL_STENCIL_ATTACHMENT, fbo.stencil.id, 0);

		std::vector<GLenum> buffers(spec.color_attachments.size());
		for(int i = 0; i<buffers.size(); i++)
			buffers[i] = GL_COLOR_ATTACHMENT0 + i;
		glNamedFramebufferDrawBuffers(fbo_name, buffers.size(), buffers.data());

		glPopDebugGroup();
	}

	void RenderPass::recreate_attribute_buffers(int max_vertices, int max_indices, int max_instances, bool preserve_data)
	{
		const char context_name[] = "recreate_attribute_buffers()";
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, __LINE__, sizeof(context_name), context_name);

		if(vertex_array_id)
			glDeleteVertexArrays(1, &vertex_array_id);
		glCreateVertexArrays(1, &vertex_array_id);

		vattribute_specs.clear();

		if(max_vertices  == -1) max_vertices  = spec.max_vertices;
		if(max_instances == -1) max_instances = spec.max_instances;
		if(max_indices   == -1) max_indices   = spec.max_indices;

		GLuint      buffers[3] = {0, 0, 0};
		GLuint  old_buffers[3] = {vertex_attributes.id, instance_attributes.id, indices.id};
		int old_buffer_size[3] = {vertex_attributes.write_offset, instance_attributes.write_offset, indices.write_offset};
		GLintptr    offsets[3] = {0, 0, 0};

		uint32_t format_hashes[3] = {0, 0, GL_UNSIGNED_INT};

		GLsizei strides[3] = {0, 0, sizeof(GLuint)};
		decltype(spec.vertex_attributes)* attrib_lists[2] = {&(spec.vertex_attributes), &(spec.instance_attributes)}; 

		GLuint vattrib_id = 0;
		for(int i = 0; i < 2; i++)
		{
			int offset = 0;
			bool is_instance_attrib = (i == 1);
			int buffer_vattrib_first_id = vattrib_id;
			for(auto& vattrib : *(attrib_lists[i]))
			{
				DataFormatStruct fmt(vattrib.format);
				int vattrib_first_id = vattrib_id;
				for(int j = 0; j < fmt.detail.num_columns * vattrib.array_elements; j++)
				{
					glVertexArrayAttribFormat(vertex_array_id, vattrib_id, fmt.detail.num_elements, fmt.get_gl_type_enum(), GL_FALSE, offset);
					glVertexArrayBindingDivisor(vertex_array_id, vattrib_id, is_instance_attrib);
					glVertexArrayAttribBinding(vertex_array_id, vattrib_id, i);
					offset += fmt.detail.size * fmt.detail.num_elements;
					vattrib_id++;
				}
				vattribute_specs.insert({vattrib.hint, {vattrib.format, vattrib_first_id, vattrib.array_elements, offset}});
				format_hashes[i] += (((vattrib_id - buffer_vattrib_first_id + 1) * fmt.detail.size) << 2) + (fmt.detail.num_elements << 1) + fmt.detail.num_columns;
			}
			strides[i] = offset;
		}

		bool compatible = true;
		compatible &= (format_hashes[0] == vertex_attributes.format_hash);
		compatible &= (format_hashes[1] == instance_attributes.format_hash);
		compatible &= (format_hashes[2] == indices.format_hash);
		if(preserve_data && !compatible)
		{	
			const char error_message[] = "Attribute format has changed, can't preserve data!";
			glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 9002, GL_DEBUG_SEVERITY_MEDIUM, sizeof(error_message), error_message);
			preserve_data = false;
		}
		
		int buffer_size[3] = {strides[0] * max_vertices, strides[1] * max_instances, strides[2] * max_indices};
		for(int i = 0; i++; i<3)
		{
			//printf("%d", buffer_size[i]);
			if(buffer_size[i] <= 0)
				continue;

			glCreateBuffers(1, &buffers[i]);
			glNamedBufferStorage(buffers[i], buffer_size[i], NULL, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);
			if(preserve_data && old_buffers[i])
			{
				int to_write = old_buffer_size[0];
				if(buffer_size[0] < to_write)
				{
					// This can be intentional, but we notify in case it is not.
					const char warning_message[] = "Buffer to copy to is smaller than buffer to copy from, existing meshes could become invalid.";
					glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR, 9001, GL_DEBUG_SEVERITY_NOTIFICATION, sizeof(warning_message), warning_message);
					to_write = buffer_size[0];
				}
				glCopyNamedBufferSubData(old_buffers[i], buffers[i], 0, 0, to_write);
			}
		}
		glVertexArrayVertexBuffers(vertex_array_id, 0, 2, buffers, offsets, strides);
		glDeleteBuffers(3, old_buffers);

		vertex_attributes   = {buffers[0], buffer_size[0], format_hashes[0], strides[0]};
		instance_attributes = {buffers[1], buffer_size[1], format_hashes[1], strides[1]};
		indices             = {buffers[2], buffer_size[2], format_hashes[2], strides[2]};
		glVertexArrayElementBuffer(vertex_array_id, indices.id);

		glPopDebugGroup();
	}
	
	void RenderPass::setup_transform_feedback(const std::vector<AttributeBuffer>& buffers) 
	{
		if(transform_buffer_id)
			tear_down_transform_feedback();

		glCreateTransformFeedbacks(1, &transform_buffer_id);

		int i = 0;
		for(auto buffer : buffers)
			glTransformFeedbackBufferBase(transform_buffer_id, i++, buffer.id);

		glBeginTransformFeedback(GL_TRIANGLES);
	}
	
	void RenderPass::tear_down_transform_feedback() 
	{
		glEndTransformFeedback();
		glDeleteTransformFeedbacks(1, &transform_buffer_id);
		transform_buffer_id = 0;
	}
	
	std::string RenderPass::run_preprocessor(std::string_view source) 
	{
		// TODO
	}
	
	GPU_InstanceData RenderPass::upload_raw_instances(uint8_t* data, size_t length) 
	{
		const char context_name[] = "upload_raw_instances()";
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, __LINE__, sizeof(context_name), context_name);

		if((!instance_attributes) && length > 0)
		{
			const char warning_message[] = "Trying to upload instance data to RenderPass without instance attribute buffer.";
			glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR, 9005, GL_DEBUG_SEVERITY_NOTIFICATION, sizeof(warning_message), warning_message);
			return {};
		}

		GPU_InstanceData retVal;
		retVal.instance_offset = instance_attributes.write_offset / instance_attributes.stride;
		retVal.num_instances = length / instance_attributes.stride;
		retVal.instance_format_hash = instance_attributes.format_hash;
		assert((length % instance_attributes.stride) == 0); // Illformed data

		instance_attributes.upload_data(data, length);

		glPopDebugGroup();
		return retVal;
	}

	void RenderPass::draw(int num_indices, int num_instances, int index_offset, int instance_offset, int vertex_offset)
	{
		const char context_name[] = "draw()";
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, __LINE__, sizeof(context_name), context_name);
		
		glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);
		glViewport(0, 0, fbo.size[0], fbo.size[1]);
		glBindVertexArray(vertex_array_id);
		glUseProgram(shader.id);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo.id);
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transform_buffer_id);

		// Do we have a index buffer?
		if(indices.id)
			glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLE_STRIP, num_indices, GL_UNSIGNED_INT, (void*)(sizeof(GLuint) * index_offset), num_instances, vertex_offset, instance_offset);
		else
			glDrawArraysInstancedBaseInstance(GL_TRIANGLE_STRIP, index_offset, num_indices, 1, instance_offset);

		glPopDebugGroup();
	}
	
	void RenderPass::draw(const GPU_MeshData& mesh, int num_instances, int instance_offset) 
	{
		draw(mesh.num_indices, num_instances, mesh.index_offset, instance_offset, mesh.vertex_offset);
	}
	
	void RenderPass::draw(const GPU_MeshData& mesh, const GPU_InstanceData& instances) 
	{
		draw(mesh.num_indices, instances.num_instances, mesh.index_offset, instances.instance_offset, mesh.vertex_offset);
	}
	
}