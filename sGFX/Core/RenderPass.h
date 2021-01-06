
#pragma once

#include "BufferObject.h"
#include "Types.h"
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
}

struct [[slick::tuple]] RenderAttachment
{
	TextureFormat format;
	int min_mip_level = 0;
	int max_mip_level = 0;
}

struct [[slick::tuple]] ShaderAttribute
{
	DataFormat format;
	UsageHint hint = UsageHint::Custom;
	int array_elements = 1;
}

struct [[slick::object]] RenderPassSpec
{
	[[slick::import_file]] std::vector<unsigned char> shader_vertex   = "";
	[[slick::import_file]] std::vector<unsigned char> shader_fragment = "";

	RenderAttachment depth_attachment = {TextureFormat::None};
	RenderAttachment stencil_attachment = {TextureFormat::None};

	std::vector<RenderAttachment> color_attachments;
	std::vector<ShaderAttribute>  vertex_attributes;
	std::vector<ShaderAttribute>  instance_attributes;
	std::vector<ShaderAttribute>  uniforms;

	// These determine the size of the buffers
	int max_vertices  = 32767; // Going over 32k will use 32 bit indices.
	int max_instances = 256;
}

struct AttributeSpec
{
	DataFormat format;
	int index;
	int array_elements;
	int byte_stride;
	int byte_offset;
}

struct CompiledRenderPass
{
	Framebuffer fbo;
	AttributeBuffer indices;
	AttributeBuffer vertex_attributes;
	AttributeBuffer instance_attributes;

	DataFormat index_format = DataFormat::Uint16;
	std::multimap<UsageHint, AttributeSpec> attribute_specs;
}

}