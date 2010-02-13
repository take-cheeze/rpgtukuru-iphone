/**
 * @file
 * @brief Scoped Array Pointer
 * @author takuto
 */
#pragma once

#include "kuto_error.h"


namespace kuto {

template<class T>
class ScopedArray
{
public:
	typedef T element_type;
	
public:
	explicit ScopedArray(T* ptr = NULL) : ptr_(ptr) {}
	~ScopedArray() { if (ptr_) delete[] ptr_; }
	
	T* get() { return ptr_; }
	const T* get() const { return ptr_; }
	void reset(T* ptr = NULL) { if (ptr_) delete[] ptr_; ptr_ = ptr; }
	T& operator*() { kuto_assert(ptr_); return *ptr_; }
	const T& operator*() const { kuto_assert(ptr_); return *ptr_; }
	T* operator->() { kuto_assert(ptr_); return ptr_; }
	const T* operator->() const { kuto_assert(ptr_); return ptr_; }
	T& operator[](int index) { kuto_assert(ptr_); return ptr_[index]; }
	const T& operator[](int index) const { kuto_assert(ptr_); return ptr_[index]; }

private:
	T& operator=(const ScopedArray&);
	ScopedArray(const ScopedArray&);
	
private:
	T*		ptr_;
};

}	// namespace kuto
