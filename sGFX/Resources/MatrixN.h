#pragma once
#include "VectorN.h"

template<int NInput, int NOutput, typename T>
struct [[slick::tuple]] MatrixN
{
	VectorN<NInput, T> rows[NOutput];

	static constexpr Matrix<NInput, NOutput, T> identity()
	{
		Matrix<NInput, NOutput, T> result;
		for(int i = 0; i<NOutput; i++)
		{
			result.rows[i] = VectorN<NInput, T>(0);
			if((i < NInput) && (i < NOutput))
				result.rows[i][i] = 1;
		}
		return result;
	}

	constexpr VectorN<NOutput, T> transform(VectorN<NInput, T> input)
	{
		VectorN<NOutput, T> out;
		for(int i = 0; i < NOutput; i++)
			out[i] = input.sum([](T a, T b){ return a * b; }, rows[i]);
		return out;
	};

	constexpr MatrixN<NInput, NOutput, T> multiply(MatrixN<NInput, NOutput, T> other)
	{
		MatrixN<NInput, NOutput, T> out;
		
	};
}