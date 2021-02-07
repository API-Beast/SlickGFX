#pragma once

#include <cmath>

namespace sGFX
{

namespace detail
{
	template<bool B, typename>
	struct enable_if{};

	template<typename T>
	struct enable_if<true, T>{ using type = T; };
}

template<bool B, typename T = void>
using enable_if_t = typename detail::enable_if<B, T>::type;

template<int N, typename T>
struct [[slick::tuple]] VectorN
{
	template <typename... Args, typename = enable_if_t<sizeof...(Args) == N> >
	constexpr VectorN(Args&&... args) : elements{(T)args...}{};
	constexpr VectorN(T scalar){ for(int i = 0; i < N; i++) elements[i] = scalar; };
	template<typename OtherT>
	constexpr VectorN(const VectorN<N, OtherT>& vector){ for(int i = 0; i < N; i++) elements[i] = vector[i]; };
	constexpr VectorN(){ for(int i = 0; i < N; i++) elements[i] = T(0); };

	T elements[N];
	constexpr T& operator[](int i){ return elements[i]; };
	constexpr const T& operator[](int i) const{ return elements[i]; };
	template<int i>
	constexpr T get(){ static_assert(i < N, "out of bounds"); return elements[i]; };

	constexpr T* begin(){ return elements; };
	constexpr T* end()  { return elements + N; };
	constexpr const T* begin() const { return elements; };
	constexpr const T* end()   const { return elements + N; };

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

	constexpr bool operator==(const VectorN<N, T>& other){ return all([](T a, T b){ return a == b; }, other);  };
	constexpr bool operator!=(const VectorN<N, T>& other){ return !operator==(other); };
	// Lexicographical comparison
	constexpr bool operator <(const VectorN<N, T>& other){ return conditional_first([](T a, T b){ return a != b; }, [](T a, T b){ return a < b; }, other); };
	constexpr bool operator >(const VectorN<N, T>& other){ return !operator<=(other); };
	constexpr bool operator>=(const VectorN<N, T>& other){ return !operator<(other); };
	constexpr bool operator<=(const VectorN<N, T>& other){ return operator<(other) || operator==(other);  };

	/* Vector specific functions */
	constexpr T square_length()         { return sum([](T a){ return a*a; });      };
	constexpr T length()                { return std::sqrt(square_length());       };
	constexpr T dot(VectorN<N, T> other){ return sum([](T a, T b){ return a*b; }); };

	/* Set of components specific functions */
	constexpr T min(){ return compare([](T a, T b){ return a < b; }); };
	constexpr T max(){ return compare([](T a, T b){ return a > b; }); };
	constexpr VectorN<N, float> min_aspect(){ return VectorN<N, float>(*this) / min(); };
	constexpr VectorN<N, float> max_aspect(){ return VectorN<N, float>(*this) / max(); };

	/* Different ways of calling functions per component. */
	template<typename Func, typename... Args>                  constexpr VectorN<N, T> visit(Func f, Args&&... args);
	template<typename Func, typename... Args>                  constexpr T sum(Func f, Args&&... args);
	template<typename Func>                                    constexpr T compare(Func f);
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
constexpr T VectorN<N, T>::sum(Func f, Args&&... args)
{
	using namespace VectorN_detail;
	T result = static_cast<T>(0);
	for(int i = 0; i < N; i++)
		result += f(elements[i], subscript<N, T>(args, i)...);
	return result;
}

template<int N, typename T>
template<typename Func>
constexpr T VectorN<N, T>::compare(Func f)
{
	using namespace VectorN_detail;
	T result = elements[0];
	for(int i = 1; i < N; i++)
		if(f(elements[i], result))
			result = elements[i];
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
constexpr bool VectorN<N, T>::any(Func f, Args&&... args)
{
	using namespace VectorN_detail;
	bool result = false;
	for(int i = 0; i < N; i++)
		result = result || f(elements[i], subscript<N, T>(args, i)...);
	return result;
}

template<int N, typename T>
template<typename FuncA, typename FuncB, typename... Args>
constexpr bool VectorN<N, T>::conditional_first(FuncA condition, FuncB evaluation, Args&&... args)
{
	using namespace VectorN_detail;
	for(int i = 0; i < N; i++)
		if(condition(elements[i], subscript(args, i)...))
			return evaluation(elements[i], subscript<N, T>(args, i)...);
	return false;
}

}