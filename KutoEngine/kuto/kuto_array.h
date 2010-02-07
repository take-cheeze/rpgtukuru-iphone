/**
 * @file
 * @brief 静的配列クラス　普通の配列より便利に安全に
 * @author takuto
 */
#pragma once

#include "kuto_types.h"
#include "kuto_error.h"


namespace kuto {

/// 静的配列クラス　普通の配列より便利に安全に
template<class T, unsigned int CAPACITY>
class Array
{
public:
	typedef T* iterator;
	typedef const T* const_iterator;
	
public:
	Array() {}
	
	iterator begin() { return buffer_; }
	const_iterator begin() const { return buffer_; }
	iterator end() { return buffer_ + CAPACITY; }
	const_iterator end() const { return buffer_ + CAPACITY; }
	
	T& front() { return buffer_[0]; }
	const T& front() const { return buffer_[0]; }
	T& back() { return buffer_[CAPACITY - 1]; }
	const T& back() const { return buffer_[CAPACITY - 1]; }
	
	u32 size() const { return CAPACITY; }
	bool empty() const { return false; }
	u32 capacity() const { return CAPACITY; }
	
	T& at(u32 index) { kuto_assert(index < CAPACITY); return buffer_[index]; }
	const T& at(u32 index) const { kuto_assert(index < CAPACITY); return buffer_[index]; }
	T& operator[](u32 index) { kuto_assert(index < CAPACITY); return buffer_[index]; }
	const T& operator[](u32 index) const { kuto_assert(index < CAPACITY); return buffer_[index]; }

	T* get() { return buffer_; }
	const T* get() const { return buffer_; }
	
	void zeromemory() { std::memset(buffer_, 0, sizeof(buffer_)); }

private:
	T		buffer_[CAPACITY];
};

}	// namespace kuto
