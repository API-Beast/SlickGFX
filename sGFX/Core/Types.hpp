#pragma once

#include <cstdint>
#include <sGFX/Resources/MatrixN.hpp>
#include <sGFX/Resources/VectorN.hpp>
#include <sGFX/Resources/Unorm.hpp>

namespace sGFX
{
	enum class DataFormat : uint16_t;

	struct DataFormatStruct
	{
		uint16_t value;
		struct Detail { uint8_t _reserved : 2, num_columns : 4,  size : 4, num_elements : 4, floating_point : 1, signbits : 1; } detail;

		constexpr DataFormatStruct(DataFormat f):detail(), value(uint16_t(f)){};
		constexpr DataFormatStruct(Detail d):value(0), detail(d){};

		uint32_t get_gl_type_enum();
	};

	enum class DataFormat : uint16_t
	{
		Float16 = DataFormatStruct({0, 1, 2, 1, 1, 0}).value,
		Float32 = DataFormatStruct({0, 1, 4, 1, 1, 0}).value,
		Float64 = DataFormatStruct({0, 1, 8, 1, 1, 0}).value,

		Int16 = DataFormatStruct({0, 1, 2, 1, 0, 1}).value,
		Int32 = DataFormatStruct({0, 1, 4, 1, 0, 1}).value,

		Uint8  = DataFormatStruct({0, 1, 1, 1, 0, 1}).value,
		Uint16 = DataFormatStruct({0, 1, 2, 1, 0, 1}).value,
		Uint32 = DataFormatStruct({0, 1, 4, 1, 0, 1}).value,

		Vec2F = DataFormatStruct({0, 1, 4, 2, 1, 0}).value,
		Vec3F = DataFormatStruct({0, 1, 4, 3, 1, 0}).value,
		Vec4F = DataFormatStruct({0, 1, 4, 4, 1, 0}).value,

		Vec2I = DataFormatStruct({0, 1, 4, 1, 0, 1}).value,
		Vec3I = DataFormatStruct({0, 1, 4, 1, 0, 1}).value,
		Vec4I = DataFormatStruct({0, 1, 4, 1, 0, 1}).value,

		Mat22 = DataFormatStruct({0, 2, 4, 2, 0, 1}).value,
		Mat23 = DataFormatStruct({0, 2, 4, 3, 0, 1}).value,
		Mat24 = DataFormatStruct({0, 2, 4, 4, 0, 1}).value,

		Mat33 = DataFormatStruct({0, 3, 4, 3, 0, 1}).value,
		Mat34 = DataFormatStruct({0, 3, 4, 4, 0, 1}).value,

		Mat44 = DataFormatStruct({0, 4, 4, 4, 0, 1}).value
	};

	using Vec2F = VectorN<2, float>;
	using Vec3F = VectorN<3, float>;
	using Vec4F = VectorN<4, float>;

	using RGBA_u8 = VectorN<4, Unorm<uint8_t> >;
	using RGBA_u16 = VectorN<4, Unorm<uint16_t> >;

	using Vec2I = VectorN<2, int>;
	using Vec3I = VectorN<3, int>;
	using Vec4I = VectorN<4, int>;

	using Mat22 = MatrixN<2, 2, float>;
	using Mat33 = MatrixN<3, 3, float>;
	using Mat44 = MatrixN<4, 4, float>;

}