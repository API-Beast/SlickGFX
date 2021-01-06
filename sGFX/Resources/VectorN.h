#pragma once
#include <type_traits>

template<int N, typename T>
struct [[slick::tuple]] VectorN
{
	constexpr VectorN(T scalar = T())
	{
		for(int i = 0; i <N; i++)
			elements[i] = scalar;
	};
	template <typename... Args, typename = typename std::enable_if<sizeof...(Args) == N>::type>
	constexpr VectorN(Args&&... args) : elements{args...}{};
	constexpr VectorN(T scalar){ for(int i; i < N; i++) elements[i] = scalar; };

	T elements[N];
	constexpr T& operator[](int i){ return elements[i]; };
	template<int i>
	constexpr T get(){ static_assert(i < N, "out of bounds"); return elements[i]; };

	/* Vector x Vector */
	constexpr VectorN<N, T>  operator +(VectorN<N, T> other){ return visit([](T  a, T b){ return a + b;  }, other); };
	constexpr VectorN<N, T>  operator -(VectorN<N, T> other){ return visit([](T  a, T b){ return a - b;  }, other); };
	constexpr VectorN<N, T>  operator *(VectorN<N, T> other){ return visit([](T  a, T b){ return a * b;  }, other); };
	constexpr VectorN<N, T>  operator /(VectorN<N, T> other){ return visit([](T  a, T b){ return a / b;  }, other); };

	constexpr VectorN<N, T>& operator+=(VectorN<N, T> other){     for_each([](T& a, T b){ return a += b; }, other); return *this; };
	constexpr VectorN<N, T>& operator-=(VectorN<N, T> other){     for_each([](T& a, T b){ return a -= b; }, other); return *this; };
	constexpr VectorN<N, T>& operator*=(VectorN<N, T> other){     for_each([](T& a, T b){ return a *= b; }, other); return *this; };
	constexpr VectorN<N, T>& operator/=(VectorN<N, T> other){     for_each([](T& a, T b){ return a /= b; }, other); return *this; };

	/* Vector x Scalar */
	constexpr VectorN<N, T>  operator -(T scalar){ return visit([](T  a, T b){ return a - b;  }, scalar); };
	constexpr VectorN<N, T>  operator *(T scalar){ return visit([](T  a, T b){ return a * b;  }, scalar); };
	constexpr VectorN<N, T>  operator +(T scalar){ return visit([](T  a, T b){ return a + b;  }, scalar); };
	constexpr VectorN<N, T>  operator /(T scalar){ return visit([](T  a, T b){ return a / b;  }, scalar); };

	constexpr VectorN<N, T>& operator+=(T scalar){     for_each([](T& a, T b){ return a += b; }, scalar); return *this; };
	constexpr VectorN<N, T>& operator-=(T scalar){     for_each([](T& a, T b){ return a -= b; }, scalar); return *this; };
	constexpr VectorN<N, T>& operator*=(T scalar){     for_each([](T& a, T b){ return a *= b; }, scalar); return *this; };
	constexpr VectorN<N, T>& operator/=(T scalar){     for_each([](T& a, T b){ return a /= b; }, scalar); return *this; };

	constexpr bool operator==(const ClassName& other){ return all([](T a, T b){ return a == b; }, other);  };
	constexpr bool operator!=(const ClassName& other){ return !operator==(other); };
	// Lexicographical comparison
	constexpr bool operator <(const ClassName& other){ return conditional_first([](T a, T b){ return a != b; }, [](T a, T b){ return a < b; }, other); };
	constexpr bool operator >(const ClassName& other){ return !operator<=(other); };
	constexpr bool operator>=(const ClassName& other){ return !operator<(other); };
	constexpr bool operator<=(const ClassName& other){ return operator<(other) || operator==(other);  };

	/* Vector specific functions */
	constexpr T square_length()         { return sum([](T a){ return a*a; });      };
	constexpr T length()                { return std::sqrt(square_length());       };
	constexpr T dot(VectorN<N, T> other){ return sum([](T a, T b){ return a*b; }); };

	/* Different ways of calling functions per component. */
	template<typename Func, typename... Args>                  constexpr VectorN<N, T> visit(Func f, Args&&... args);
	template<typename Func, typename... Args>                  constexpr T sum(Func f, Args&&... args);
	template<typename Func, typename... Args>                  constexpr void for_each(Func f, Args&&... args);
	template<typename Func, typename... Args>                  constexpr bool all(Func f, Args&&... args);
	template<typename Func, typename... Args>                  constexpr bool any(Func f, Args&&... args);
	template<typename FuncA, typename FuncB, typename... Args> constexpr bool conditional_first(FuncA condition, FuncB evaluation, Args&&... args);
};

namespace VectorN_detail
{

	template<int N, typename T>
	constexpr T subscript(T val, int i)
	{
		return val;
	};

	template<int N, typename T>
	constexpr T subscript(VectorN<N, T> val, int i)
	{
		return val[i];
	};
}

template<int N, typename T>
template<typename Func, typename... Args>
constexpr VectorN<N, T> VectorN<N, T>::visit(Func f, Args&&... args)
{
	using namespace VectorN_detail;
	VectorN<N, T> result;
	for(int i = 0; i < N; i++)
		result[i] = f(elements[i], subscript<N, T>(args, i)...);
	return result;
}

template<int N, typename T>
template<typename Func, typename... Args>
constexpr VectorN<N, T> VectorN<N, T>::sum(Func f, Args&&... args)
{
	using namespace VectorN_detail;
	T result = static_cast<T>(0);
	for(int i = 0; i < N; i++)
		T += f(elements[i], subscript<N, T>(args, i)...);
	return result;
}

template<int N, typename T>
template<typename Func, typename... Args>
constexpr void VectorN<N, T>::for_each(Func f, Args&&... args)
{
	using namespace VectorN_detail;
	for(int i = 0; i < N; i++)
		f(elements[i], subscript<N, T>(args, i)...);
}

template<int N, typename T>
template<typename Func, typename... Args>
constexpr bool VectorN<N, T>::all(Func f, Args&&... args)
{
	using namespace VectorN_detail;
	bool result = true;
	for(int i = 0; i < N; i++)
		result = result && f(elements[i], subscript<N, T>(args, i)...);
	return result;
}

template<int N, typename T>
template<typename Func, typename... Args>
constexpr bool VectorN<N, T>::any(Func f, Args&&... args) {
{
	using namespace VectorN_detail;
	bool result = false;
	for(int i = 0; i < N; i++)
		result = result || f(elements[i], subscript<N, T>(args, i)...);
	return result;
}

template<int N, typename T>
template<typename FuncA, typename FuncB, typename... Args>
constexpr bool VectorN<N, T>::conditional_first(FuncA condition, FuncB evaluation, Args&&... args) {
{
	using namespace VectorN_detail;
	for(int i = 0; i < N; i++)
		if(condition(elements[i], subscript(args, i)...))
			return evaluation(elements[i], subscript<N, T>(args, i)...);
	return false;
}