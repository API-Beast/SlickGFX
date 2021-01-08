
#pragma once

#include "Types.hpp"
#include "Framebuffer.hpp"
#include <vector>
#include <map>

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
	// Max indices will be always 8 times the max vertices
	int max_instances = 256;
};

struct AttributeSpec
{
	DataFormat format;
	int index;
	int array_elements;
	int byte_stride;
	int byte_offset;
};

struct RenderPass
{
	RenderPass(const RenderPassSpec& spec, int width, int height);
	void draw(int num_indices, int num_instances, int index_offset = 0, int instance_offset = 0, int vertex_offset = 0);

	ShaderProgram shader;
	Framebuffer fbo;
	AttributeBuffer indices;
	AttributeBuffer vertex_attributes;
	AttributeBuffer instance_attributes;
	unsigned int vertex_array_id;

	DataFormat index_format = DataFormat::Uint16;
	std::multimap<UsageHint,   AttributeSpec> attribute_specs;
};

}