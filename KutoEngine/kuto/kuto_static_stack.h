/**
 * @file
 * @brief Static Buffer Stack
 * @author takuto
 */
#pragma once

#include "kuto_types.h"
#include "kuto_error.h"


namespace kuto {

template<class T, uint CAPACITY>
class StaticStack
{
public:
	StaticStack() : size_(0) {}
	
	void push(const T& value) { kuto_assert(size_ < CAPACITY); buffer_[size_++] = value; }
	void pop() { kuto_assert(size_ > 0); size_--; }
	const T& top() const { kuto_assert(size_ > 0); return buffer_[size_ - 1]; }
	T& top() { kuto_assert(size_ > 0); return buffer_[size_ - 1]; }
	
	u32 size() const { return size_; }
	bool empty() const { return size_ == 0; }
	u32 capacity() const { return CAPACITY; }
	void clear() { size_ = 0; }

private:
	T		buffer_[CAPACITY];
	u32		size_;
};

}	// namespace kuto
