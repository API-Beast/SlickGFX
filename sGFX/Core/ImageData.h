#pragma once

#include <sGFX/Core/Types.hpp>

#include <new>
#include <cstring>
#include <type_traits>
#include <cassert>
#include <memory>
#include <initializer_list>

namespace sGFX
{

constexpr int ImageData_SIMD_Alignment = 32;

template<typename T>
struct [[nodiscard]] ImageData
{
	T* data = nullptr;
	int data_length = 0;
	Vec2I size = {0, 0};
	bool invalid = true;

	ImageData()=default;
	ImageData(int w, int h){ allocate(w, h); };
	~ImageData(){ clear(); };

	// Returns a single white pixel.
	static ImageData<T> fallback_image();

	ImageData(ImageData<T>&& other) noexcept;
	ImageData<T>& operator=(ImageData<T>&& other) noexcept;

	ImageData(const ImageData<T>& other);
	ImageData<T>& operator=(const ImageData<T>& other);

	void copy_from(const ImageData<T>& other);

	ImageData<T> make_copy(){ ImageData<T> retVal; retVal.copy_from(*this); return retVal; };

	void allocate(int w, int h);
	void allocate(int w, int h, T default_value);
	void allocate(Vec2I size){ return allocate(size[0], size[1]); };
	void allocate(Vec2I size, T default_value){ return allocate(size[0], size[1], default_value); };
	void clear();
	constexpr int pixel_offset(){ return sizeof(T); };
	int row_offset() const { return sizeof(T) * size[0]; };

	constexpr bool is_valid() const { return data != nullptr && !invalid; };
	constexpr operator bool(){ return is_valid(); };

	const T& operator[](int i) const{ return *(data+i); };
	      T& operator[](int i)      { return *(data+i); };

	const T& at(int x, int y) const{ return *(data+x+y*size[0]); }
	      T& at(int x, int y)      { return *(data+x+y*size[0]); }

	const T& at(Vec2I pos) const{ return at(pos[0], pos[1]); }
	      T& at(Vec2I pos)      { return at(pos[0], pos[1]); }

	T* row(int y){ return data + std::fmax(std::fmin(y, size[1]), 0) * size[0]; };
	int row_offset(){ return size[0]; };

	T* begin(){ return data; };
	T* end()  { return data + data_length; };
	const T* begin() const { return data; };
	const T* end()   const { return data + data_length; };
};

template<typename T>
void ImageData<T>::allocate(int w, int h) 
{
	clear();
	data_length = w*h;
	data = (T*)::operator new(data_length * sizeof(T), std::align_val_t(ImageData_SIMD_Alignment));
	size[0] = w;
	size[1] = h;
	invalid = false;
}

template<typename T>
void ImageData<T>::allocate(int w, int h, T default_value) 
{
	allocate(w, h);
	std::uninitialized_fill_n(begin(), w*h, default_value);
}

template<typename T>
void ImageData<T>::clear() 
{
	::operator delete(data, std::align_val_t(ImageData_SIMD_Alignment));
}

template<typename T>
ImageData<T> ImageData<T>::fallback_image() 
{
	ImageData<T> retVal;
	retVal.allocate(1, 1, T(1));
	retVal.invalid = true;
	return retVal;
}

template<typename T>
ImageData<T>::ImageData(ImageData<T>&& other) noexcept
{
	this->operator=(std::move(other));
}

template<typename T>
ImageData<T>::ImageData(const ImageData<T>& other) 
{
	copy_from(other);
}

template<typename T>
ImageData<T>& ImageData<T>::operator=(ImageData<T>&& other)  noexcept
{
	if(other.data)
	{
		size = other.size;
		data = other.data;
		data_length = other.data_length;
		other.data = nullptr;
	}
	return *this;	
}

template<typename T>
ImageData<T>& ImageData<T>::operator=(const ImageData<T>& other) 
{
	copy_from(other);
	return *this;	
}

template<typename T>
void ImageData<T>::copy_from(const ImageData<T>& other)
{
	if(other.data)
	{
		allocate(other.size[0], other.size[1]);
		T* ptr = data;
		for(auto it = other.begin(); it != other.end(); it++)
		{
			(*ptr) = (*it); 
			ptr++;
		}
	}
}

}