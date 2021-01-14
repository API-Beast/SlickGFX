#pragma once

#include <limits>

namespace sGFX
{

	template<typename T>
	struct Unorm
	{
		public:
			T data;
			static constexpr T MinV = std::numeric_limits<T>::min();
			static constexpr T MaxV = std::numeric_limits<T>::max();

			constexpr explicit operator float(){ return data / MaxV; };
			constexpr operator T(){ return data; };
		private:
			constexpr T clamp_f(float v)
			{
				return (v < MinV) ? MinV : (MaxV < v) ? MaxV : T(v);
			};
			constexpr T clamp_i(long long int v)
			{
				return (v < MinV) ? MinV : (MaxV < v) ? MaxV : v;
			};

		public:
			constexpr Unorm(T d){ data = d; };
			constexpr Unorm(float scalar){ data = clamp_f(scalar * MaxV); };

			constexpr bool operator==(Unorm<T> other){ return this->data == other.data; };
			constexpr bool operator!=(Unorm<T> other){ return this->data != other.data; };
			constexpr bool operator <(Unorm<T> other){ return this->data  < other.data; };
			constexpr bool operator >(Unorm<T> other){ return this->data  > other.data; };
			constexpr bool operator>=(Unorm<T> other){ return this->data >= other.data; };
			constexpr bool operator<=(Unorm<T> other){ return this->data <= other.data; };

			constexpr Unorm<T> operator+(float scalar){ return {clamp_f(data + (scalar * MaxV))}; };
			constexpr Unorm<T> operator-(float scalar){ return {clamp_f(data - (scalar * MaxV))}; };
			constexpr Unorm<T> operator*(float scalar){ return {clamp_f(data * (scalar * MaxV))}; };
			constexpr Unorm<T> operator/(float scalar){ return {clamp_f(data / (scalar * MaxV))}; };

			constexpr Unorm<T>& operator+=(float scalar){ return data = clamp_f(data + (scalar * MaxV)); };
			constexpr Unorm<T>& operator-=(float scalar){ return data = clamp_f(data - (scalar * MaxV)); };
			constexpr Unorm<T>& operator*=(float scalar){ return data = clamp_f(data * (scalar * MaxV)); };
			constexpr Unorm<T>& operator/=(float scalar){ return data = clamp_f(data / (scalar * MaxV)); };

			constexpr Unorm<T> operator+(T value){ return {clamp_i(data + value)}; };
			constexpr Unorm<T> operator-(T value){ return {clamp_i(data - value)}; };
			constexpr Unorm<T> operator*(T value){ return {clamp_i(data * value)}; };
			constexpr Unorm<T> operator/(T value){ return {clamp_i(data / value)}; };

			constexpr Unorm<T>& operator+=(T value){ return data = clamp_i(data + value); };
			constexpr Unorm<T>& operator-=(T value){ return data = clamp_i(data - value); };
			constexpr Unorm<T>& operator*=(T value){ return data = clamp_i(data * value); };
			constexpr Unorm<T>& operator/=(T value){ return data = clamp_i(data / value); };
	};

	
}