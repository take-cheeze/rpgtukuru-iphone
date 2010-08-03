/**
 * @file
 * @brief Scoped Pointer
 * @author project.kuto
 */
#pragma once

#include "kuto_error.h"


namespace kuto {

template<class T>
class ScopedPtr
{
public:
	typedef T element_type;

public:
	explicit ScopedPtr(T* ptr = NULL) : ptr_(ptr) {}
	~ScopedPtr() { if (ptr_) delete ptr_; }

	T* get() { return ptr_; }
	const T* get() const { return ptr_; }
	void reset(T* ptr = NULL) { if (ptr_) delete ptr_; ptr_ = ptr; }
	T& operator*() { kuto_assert(ptr_); return *ptr_; }
	const T& operator*() const { kuto_assert(ptr_); return *ptr_; }
	T* operator->() { kuto_assert(ptr_); return ptr_; }
	const T* operator->() const { kuto_assert(ptr_); return ptr_; }

private:
	T& operator=(const ScopedPtr&);
	ScopedPtr(const ScopedPtr&);

private:
	T*		ptr_;
};

}	// namespace kuto
