#pragma once

#include <fstream>
#include <vector>
#include <iterator>

namespace slick
{

inline std::vector<unsigned char> import_file(const char* file_name)
{
	std::ifstream file(file_name, std::ios::binary);
	std::vector<unsigned char> buffer{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
	buffer.push_back(0x0); // Null terminator
	return buffer;
}

}