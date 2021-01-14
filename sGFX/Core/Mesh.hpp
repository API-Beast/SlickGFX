#pragma once

#include "Types.hpp"
#include <vector>

namespace sGFX
{

	struct GPU_MeshData
	{
		int index_offset = -1;
		int attribute_offset = -1;

		int num_indices = -1;

		int vertex_format_hash = -1;
	};

	struct GPU_InstanceData
	{
		int attribute_offset = -1;
		int num_instances = -1;

		int instance_format_hash = -1;
	};

	struct CPU_InstanceData
	{
		std::vector<Mat44> transform;
		std::vector<Vec4F> color;
		std::vector<int>   texture_index;
		std::vector<Vec2F> uv_offset;
		std::vector<Vec2F> uv_size;
	};

	struct CPU_MeshData
	{
		std::vector<Vec3F>    vertex_position;
		std::vector<Vec2F>    vertex_uv;
		std::vector<Vec4F>    vertex_color;
		std::vector<uint32_t> index;
	};

}