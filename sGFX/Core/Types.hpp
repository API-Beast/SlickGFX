#pragma once

#include <sGFX/Resources/MatrixN.hpp>
#include <sGFX/Resources/VectorN.hpp>

namespace sGFX
{
	enum class TextureFormat
	{
		None,
		Lum_u8,
		Lum_f16,
		Lum_f32,
		RGB_u8,
		RGB_f16,
		RGB_f32,
		RGBA_u8,
		RGBA_f16,
		RGBA_f32
	};

	enum class DataFormat
	{
		Float16,
		Float32,
		Float64,

		Int16,
		Int32,

		Uint8,
		Uint16,
		Uint32,

		Vec2F,
		Vec3F,
		Vec4F,

		Vec2I,
		Vec3I,
		Vec4I,

		Mat22,
		Mat33,
		Mat44
	};

	using Vec2F = VectorN<2, float>;
	using Vec3F = VectorN<3, float>;
	using Vec4F = VectorN<4, float>;

	using Vec2I = VectorN<2, int>;
	using Vec3I = VectorN<3, int>;
	using Vec4I = VectorN<4, int>;

	using Mat2x2 = MatrixN<2, 2, float>;
	using Mat3x3 = MatrixN<3, 3, float>;
	using Mat4x4 = MatrixN<4, 4, float>;

	struct ShaderProgram
	{
		int id = 0;
	};

	struct AttributeBuffer
	{
		int id = 0;
	};

}