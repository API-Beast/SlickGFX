#pragma once

#include <sGFX/Core.hpp>
#include "yestest.h"

template<int N, typename T>
void print_value(const sGFX::VectorN<N, T>& f) { printf("{"); f.for_each(&YesTest::print_value); printf("}"); }

template<int N, typename T>
void ulp_difference(const sGFX::VectorN<N, T>& a, const sGFX::VectorN<N, T>& b)
{
	return a.sum(&YesTest::ulp_difference, b);
}