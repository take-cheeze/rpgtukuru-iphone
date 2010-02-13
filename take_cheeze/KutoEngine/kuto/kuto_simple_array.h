/**
 * @file
 * @brief Simple Array
 * @author takuto
 */
#pragma once

#include "kuto_types.h"
#include "kuto_error.h"


namespace kuto {

template<class T>
class SimpleArray
{
public:
	typedef T* iterator;
	typedef const T* const_iterator;
	
public:
	SimpleArray() : buffer_(NULL), size_(0) {}
	SimpleArray(u32 size) : buffer_(NULL), size_(0) { allocate(size); }
	~SimpleArray() { deallocate(); }
	
	iterator begin() { return buffer_; }
	const_iterator begin() const { return buffer_; }
	iterator end() { return buffer_ + size_; }
	const_iterator end() const { return buffer_ + size_; }
	
	T& front() { kuto_assert(size_ > 0); return buffer_[0]; }
	const T& front() const { kuto_assert(size_ > 0); return buffer_[0]; }
	T& back() { kuto_assert(size_ > 0); return buffer_[size_ - 1]; }
	const T& back() const { kuto_assert(size_ > 0); return buffer_[size_ - 1]; }
	
	u32 size() const { return size_; }
	bool empty() const { return size_ == 0; }
	u32 capacity() const { return size_; }
	
	T& at(u32 index) { kuto_assert(index < size_); return buffer_[index]; }
	const T& at(u32 index) const { kuto_assert(index < size_); return buffer_[index]; }
	T& operator[](u32 index) { kuto_assert(index < size_); return buffer_[index]; }
	const T& operator[](u32 index) const { kuto_assert(index < size_); return buffer_[index]; }
	
	void allocate(u32 size) {
		if (buffer_ != NULL)
			deallocate();
		buffer_ = new T[size];
		size_ = size;
	}
	void deallocate() {
		if (buffer_ != NULL) {
			delete[] buffer_;
			buffer_ = NULL;
			size_ = 0;
		}
	}
	
private:
	T*		buffer_;
	u32		size_;
};	// class SimpleArray

}	// namespace kuto
