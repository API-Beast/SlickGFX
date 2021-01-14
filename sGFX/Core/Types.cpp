#include "Types.hpp"
#include "GL/flextGL.h"

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
	
	bool AttributeBuffer::upload_data(uint8_t* data, int length) 
	{
		if(length > (capacity - write_offset))
		{
			const char error_message[] = "AttributeBuffer is full, can't write data. Do you need to resize?";
			glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 9003, GL_DEBUG_SEVERITY_LOW, sizeof(error_message), error_message);
			return false;
		}

		glNamedBufferSubData(id, write_offset, length, (void*)data);
		write_offset += length;
		return true;
	}
	
	bool AttributeBuffer::replace_data(int offset, uint8_t* data, int length) 
	{
		if(length > (capacity - offset))
		{
			const char error_message[] = "Trying to write beyond last element of AttributeBuffer.";
			glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 9004, GL_DEBUG_SEVERITY_LOW, sizeof(error_message), error_message);
			return false;
		}

		glNamedBufferSubData(id, offset, length, (void*)data);
		offset += length;
		if(offset > write_offset)
			write_offset = offset;
		return true;
	}
}