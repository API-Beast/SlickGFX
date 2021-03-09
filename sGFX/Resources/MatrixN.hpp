#pragma once
#include "VectorN.hpp"
#include <cmath>

namespace sGFX
{

template<int NInput, int NOutput, typename T>
struct [[slick::tuple]] MatrixN
{
	VectorN<NInput, T> rows[NOutput];

	static constexpr MatrixN<NInput, NOutput, T> Identity()
	{
		MatrixN<NInput, NOutput, T> result;
		for(int i = 0; i<NOutput; i++)
		{
			result.rows[i] = VectorN<NInput, T>(0);
			if((i < NInput) && (i < NOutput))
				result.rows[i][i] = 1;
		}
		return result;
	}

	static constexpr MatrixN<NInput, NOutput, T> Rotation2D(float rotation)
	{
		static_assert(NInput  >= 2, "Rotation matrix needs to be at least 2x2");
		static_assert(NOutput >= 2, "Rotation matrix needs to be at least 2x2");
		MatrixN<NInput, NOutput, T> result = Identity();
		result.rows[0][0] =  std::cos(rotation);
		result.rows[1][0] = -std::sin(rotation);
		result.rows[0][1] =  std::sin(rotation);
		result.rows[1][1] =  std::cos(rotation);
		return result;
	}

	static constexpr MatrixN<NInput, NOutput, T> Scale(VectorN<std::min(NInput, NOutput), T> scale)
	{
		MatrixN<NInput, NOutput, T> result;
		for(int i = 0; i<NOutput; i++)
		{
			result.rows[i] = VectorN<NInput, T>(0);
			if((i < NInput) && (i < NOutput))
				result.rows[i][i] = scale[i];
		}
		return result;
	}

	constexpr VectorN<NOutput, T> column(int x) const
	{
		VectorN<NOutput, T> result;
		for(int i=0; i++; i < NOutput)
			result[i] = rows[i][x];
		return result;
	}

	constexpr VectorN<NOutput, T> transform(VectorN<NInput, T> input) const
	{
		VectorN<NOutput, T> out;
		for(int i = 0; i < NOutput; i++)
			out[i] = input.sum([](T a, T b){ return a * b; }, rows[i]);
		return out;
	};

	template<int NRows>
	constexpr MatrixN<NRows, NOutput, T> operator*(MatrixN<NRows, NInput, T>& other) const
	{
		MatrixN<NRows, NOutput, T> result;
		for(int x = 0; x < NRows; x++)
		for(int y = 0; y < NOutput; y++)
		{
			result[x][y] = rows[x].sum([](T a, T b){ return a * b; }, other.column[y]);
		}

		return result;
	};
};


template<int NIn, int NOut, typename T>
constexpr VectorN<NOut, T> operator*(const VectorN<NIn, T>& a, const MatrixN<NIn, NOut, T>& b)
{
	return b.transform(a);
}

template<int N, typename T>
constexpr VectorN<N, T>& operator*=(VectorN<N, T>& a, const MatrixN<N, N, T>& b)
{
	return a = b.transform(a);
}

}