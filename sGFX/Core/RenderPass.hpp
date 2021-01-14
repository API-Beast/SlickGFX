
#pragma once

#include "Types.hpp"
#include "Framebuffer.hpp"
#include "Mesh.hpp"
#include "ShaderProgram.hpp"
#include <vector>
#include <map>
#include <initializer_list>

namespace sGFX
{

enum class UsageHint
{
	Custom,
	VertexPosition,
	VertexColor,
	VertexTexCoords,

	InstanceColor,
	InstanceTransform,
	// For multiple bound textures
	InstanceTextureIndex,
	// For texture atlases
	InstanceTextureUVOffset,
	InstanceTextureUVFactor,
};

struct [[slick::tuple]] RenderAttachment
{
	TextureFormat format;
};

struct [[slick::tuple]] ShaderAttribute
{
	DataFormat format;
	UsageHint hint = UsageHint::Custom;
	int array_elements = 1;
};

struct [[slick::object]] RenderPassSpec
{
	[[slick::import_file]] std::vector<unsigned char> shader_vertex;
	[[slick::import_file]] std::vector<unsigned char> shader_fragment;

	RenderAttachment depth_attachment = {TextureFormat::None};
	RenderAttachment stencil_attachment = {TextureFormat::None};

	std::vector<RenderAttachment> color_attachments;
	std::vector<ShaderAttribute>  vertex_attributes;
	std::vector<ShaderAttribute>  instance_attributes;

	// These determine the size of the buffers
	int max_vertices  = 32767; // Going over 32k will use 32 bit indices.
	int max_indices   = 32767 * 8;
	int max_instances = 256;
};

struct AttributeSpec
{
	DataFormat format;
	int index;
	int array_elements;
	int byte_offset;
}; 

struct RenderPass
{
	RenderPass(){};
	~RenderPass();

	void draw(int num_indices, int num_instances, int index_offset = 0, int instance_offset = 0, int vertex_offset = 0);
	void draw(const GPU_MeshData&);

	void create_from_spec(const RenderPassSpec& spec, int width, int height);
	void recreate_shader();
	void recreate_framebuffer(int width, int height);
	void recreate_attribute_buffers(int max_vertices = -1, int max_indices = -1, int max_instances = -1, bool preserve_data = false);

	/*GPU_MeshData     upload(const CPU_MeshData& data);
	GPU_InstanceData upload(const CPU_InstanceData& data);
	GPU_MeshData     upload_raw_vertices (uint8_t* data, size_t length, uint32_t* indices = nullptr, size_t index_length = 0);*/
	GPU_InstanceData upload_raw_instances(uint8_t* data, size_t length);

	RenderPassSpec spec;
	ShaderProgram shader;
	Framebuffer fbo;
	AttributeBuffer indices;
	AttributeBuffer vertex_attributes;
	AttributeBuffer instance_attributes;
	unsigned int vertex_array_id;

	std::multimap<UsageHint,   AttributeSpec> vattribute_specs;
};

}