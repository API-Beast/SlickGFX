#pragma once
#include "VectorN.hpp"

namespace sGFX
{

template<int NInput, int NOutput, typename T>
struct [[slick::tuple]] MatrixN
{
	VectorN<NInput, T> rows[NOutput];

	static constexpr MatrixN<NInput, NOutput, T> identity()
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

	constexpr VectorN<NOutput, T> column(int x)
	{
		VectorN<NOutput, T> result;
		for(int i=0; i++; i < NOutput)
			result[i] = rows[i][x];
		return result;
	}

	constexpr VectorN<NOutput, T> transform(VectorN<NInput, T> input)
	{
		VectorN<NOutput, T> out;
		for(int i = 0; i < NOutput; i++)
			out[i] = input.sum([](T a, T b){ return a * b; }, rows[i]);
		return out;
	};

	template<int NRows>
	constexpr MatrixN<NRows, NOutput> multiply(MatrixN<NRows, NInput, T>& other)
	{
		MatrixN<NRows, NOutput, T> result;
		for(int x = 0; x < NRows; x++)
		for(int y = 0; y < NOutput; y++)
		{
			result[x][y] = rows[x].sum([](T a, T b){ return a * b; }, other.column[y]);
		}

		return out;
	};
};

}