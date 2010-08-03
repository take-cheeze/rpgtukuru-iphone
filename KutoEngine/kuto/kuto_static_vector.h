/**
 * @file
 * @brief Static Buffer Vector
 * @author project.kuto
 */
#pragma once

#include "kuto_types.h"
#include "kuto_error.h"


namespace kuto {

template<class T, uint CAPACITY>
class StaticVector
{
public:
	typedef T* iterator;
	typedef const T* const_iterator;

public:
	StaticVector() : size_(0) {}

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
	u32 capacity() const { return CAPACITY; }

	T& at(u32 index) { kuto_assert(index < size_); return buffer_[index]; }
	const T& at(u32 index) const { kuto_assert(index < size_); return buffer_[index]; }
	T& operator[](u32 index) { kuto_assert(index < size_); return buffer_[index]; }
	const T& operator[](u32 index) const { kuto_assert(index < size_); return buffer_[index]; }

	void push_back(const T& value) { kuto_assert(size_ < CAPACITY); buffer_[size_++] = value; }
	void pop_back() { kuto_assert(size_ > 0); size_--; }
	void clear() { size_ = 0; }
	void resize(u32 size) { size_ = size; }

	iterator insert(iterator it, const T& value) {
		u32 index = it - buffer_;
		kuto_assert(index <= size_);
		kuto_assert(size_ < CAPACITY);
		size_++;
		for (u32 i = size_ - 1; i > index; i--)
			buffer_[i] = buffer_[i - 1];
		buffer_[index] = value;
		return buffer_ + index;
	}
	iterator erase(iterator it) {
		u32 index = it - buffer_;
		kuto_assert(index < size_);
		size_--;
		for (u32 i = index; i < size_; i++)
			buffer_[i] = buffer_[i + 1];
		return buffer_ + index;
	}

private:
	T		buffer_[CAPACITY];
	u32		size_;
};

}	// namespace kuto
