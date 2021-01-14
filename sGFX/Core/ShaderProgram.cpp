#include "ShaderProgram.hpp"
#include <sGFX/Core/GL/flextGL.h>

namespace sGFX
{
	void ShaderProgram::set_uniform(uint32_t location, int val) 
	{
		glProgramUniform1f(id, location, val);
	}
	
	void ShaderProgram::set_uniform(uint32_t location, Vec2I val) 
	{
		glProgramUniform2f(id, location, val[0], val[1]);
	}
	
	void ShaderProgram::set_uniform(uint32_t location, Vec3I val) 
	{
		glProgramUniform3f(id, location, val[0], val[1], val[2]);
	}
	
	void ShaderProgram::set_uniform(uint32_t location, Vec4I val) 
	{
		glProgramUniform4f(id, location, val[0], val[1], val[2], val[3]);
	}
	
	void ShaderProgram::set_uniform(uint32_t location, float val) 
	{
		glProgramUniform1f(id, location, val);
	}
	
	void ShaderProgram::set_uniform(uint32_t location, Vec2F val) 
	{
		glProgramUniform2f(id, location, val[0], val[1]);
	}
	
	void ShaderProgram::set_uniform(uint32_t location, Vec3F val) 
	{
		glProgramUniform3f(id, location, val[0], val[1], val[2]);
	}
	
	void ShaderProgram::set_uniform(uint32_t location, Vec4F val) 
	{
		glProgramUniform4f(id, location, val[0], val[1], val[2], val[3]);
	}
}