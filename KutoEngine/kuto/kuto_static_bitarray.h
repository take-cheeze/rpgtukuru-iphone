/**
 * @file
 * @brief Static Buffer Vector
 * @author takuto
 */
#pragma once

#include <cstring>
#include "kuto_types.h"
#include "kuto_error.h"


namespace kuto {

template<uint CAPACITY>
class StaticBitArray
{
public:
	enum {
		BUFFER_SIZE = CAPACITY / 32 + 1,
	};

public:
	StaticBitArray() { reset(); }
	u32 capacity() const { return CAPACITY; }
	void set() { std::memset(buffer_, 0xFF, sizeof(buffer_)); }
	void set(u32 index) { set(index, true); }
	void set(u32 index, bool value) {
		kuto_assert(index < CAPACITY);
		if (value)
			buffer_[index / 32] |= (1 << (index % 32));
		else
			buffer_[index / 32] &= ~(1 << (index % 32));
	}
	void reset() { std::memset(buffer_, 0, sizeof(buffer_)); }
	void reset(u32 index) { set(index, false); }
	bool get(u32 index) const { kuto_assert(index < CAPACITY); return (buffer_[index / 32] & (1 << (index % 32))) != 0; }

private:
	u32		buffer_[BUFFER_SIZE];
};

}	// namespace kuto
