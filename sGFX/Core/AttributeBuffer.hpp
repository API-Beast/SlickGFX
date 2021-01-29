#pragma once

#include <sGFX/Core/Types.hpp>

namespace sGFX
{
	struct AttributeBuffer
	{
		enum Access
		{
			Neither = 0,
			Read = 1,
			Write = 2
		};

		uint32_t id = 0;
		int capacity = 0;
		int format_hash = 0;
		int stride = 0;
		int write_offset = 0;
		int access = Neither;

		static AttributeBuffer SetupUploadBuffer(int num_bytes);
		static AttributeBuffer SetupReadBuffer(int num_bytes);
		// Read+Write buffer is a anti-pattern
		
		void tear_down();

		bool upload_data(uint8_t* data, int length);
		bool replace_data(int offset, uint8_t* data, int length);
		
		void* map_to_memory_readonly();
		void* map_to_memory_writeonly();
		bool unmap();
		
		operator bool(){ return id; };
		bool operator==(const AttributeBuffer& other){ return id == other.id; };
	};
}