#include "AttributeBuffer.hpp"

#include "GL/flextGL.h"

namespace sGFX
{
	AttributeBuffer AttributeBuffer::SetupUploadBuffer(int num_bytes) 
	{
		GLuint id;
		glCreateBuffers(1, &id);
		glNamedBufferStorage(id, num_bytes, NULL, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);
		return AttributeBuffer{id, num_bytes, 0, 0, 0, AttributeBuffer::Write};
	}
	
	AttributeBuffer AttributeBuffer::SetupReadBuffer(int num_bytes) 
	{
		GLuint id;
		glCreateBuffers(1, &id);
		glNamedBufferStorage(id, num_bytes, NULL, GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
		return AttributeBuffer{id, num_bytes, 0, 0, 0, AttributeBuffer::Read};
	}
	
	void AttributeBuffer::tear_down() 
	{
		glDeleteBuffers(1, &id);
	}

	bool AttributeBuffer::upload_data(uint8_t* data, int length) 
	{
		if(!(access&AttributeBuffer::Write))
		{
			const char error_message[] = "Trying to write to AttributeBuffer without write permissions.";
			glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 9005, GL_DEBUG_SEVERITY_LOW, sizeof(error_message), error_message);
			return false;
		}
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
		if(!(access&AttributeBuffer::Write))
		{
			const char error_message[] = "Trying to write to AttributeBuffer without write permissions.";
			glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 9005, GL_DEBUG_SEVERITY_LOW, sizeof(error_message), error_message);
			return false;
		}
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
	
	void* AttributeBuffer::map_to_memory_readonly() 
	{
		return glMapNamedBuffer(id, GL_READ_ONLY);
	}
	
	void* AttributeBuffer::map_to_memory_writeonly() 
	{
		return glMapNamedBuffer(id, GL_WRITE_ONLY);
	}
	
	bool AttributeBuffer::unmap() 
	{
		glUnmapNamedBuffer(id);
	}
}