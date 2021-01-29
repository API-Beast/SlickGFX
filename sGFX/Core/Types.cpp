#include <sGFX/Core/Types.hpp>
#include <sGFX/Core/GL/flextGL.h>

namespace sGFX
{
	uint32_t DataFormatStruct::get_gl_type_enum() 
	{
		if(detail.floating_point)
		{
			if(detail.size == sizeof(GLhalf)) return GL_HALF_FLOAT;
			if(detail.size == sizeof(GLfloat)) return GL_FLOAT;
			if(detail.size == sizeof(GLdouble)) return GL_DOUBLE;
		}
		else if(detail.signbits)
		{
			if(detail.size == sizeof(GLbyte)) return GL_BYTE;
			if(detail.size == sizeof(GLshort)) return GL_SHORT;
			if(detail.size == sizeof(GLint)) return GL_INT;
		}
		else
		{
			if(detail.size == sizeof(GLubyte)) return GL_UNSIGNED_BYTE;
			if(detail.size == sizeof(GLushort)) return GL_UNSIGNED_SHORT;
			if(detail.size == sizeof(GLuint)) return GL_UNSIGNED_INT;
		}
		return GL_FALSE;
	}
}