#pragma once

#include <sGFX/Core/Types.hpp>

namespace sGFX
{

struct ShaderProgram
{
	uint32_t id = 0;

	void set_uniform(uint32_t location, int val);
	void set_uniform(uint32_t location, Vec2I val);
	void set_uniform(uint32_t location, Vec3I val);
	void set_uniform(uint32_t location, Vec4I val);

	void set_uniform(uint32_t location, float val);
	void set_uniform(uint32_t location, Vec2F val);
	void set_uniform(uint32_t location, Vec3F val);
	void set_uniform(uint32_t location, Vec4F val);

	operator bool(){ return id; };
	bool operator==(const ShaderProgram& other){ return id == other.id; };
};

}