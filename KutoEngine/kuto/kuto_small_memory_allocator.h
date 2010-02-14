/**
 * @file
 * @brief Small Memory Allocator
 * @author project.kuto
 */
#pragma once

#include "kuto_types.h"
#include "kuto_error.h"
#include "kuto_static_memory_allocator.h"


namespace kuto {

class SmallMemoryAllocator
{
public:
	SmallMemoryAllocator() {}
	
	u32 maxAllocSize() const { return 64; }
	
	char* alloc(u32 size) {
		kuto_assert(size <= maxAllocSize());
		if (size <= 8) {
			return allocator8_.alloc();
		} else if (size <= 16) {
			return allocator16_.alloc();
		} else if (size <= 32) {
			return allocator32_.alloc();
		} else if (size <= 48) {
			return allocator48_.alloc();
		} else {
			return allocator64_.alloc();
		}
	}
	
	bool free(void* buffer) {
		if (allocator8_.free(buffer))
			return true;
		if (allocator16_.free(buffer))
			return true;
		if (allocator32_.free(buffer))
			return true;
		if (allocator48_.free(buffer))
			return true;
		if (allocator64_.free(buffer))
			return true;
		return false;
	}
	
	void print() {
		kuto_printf("  small alloc :  8 bytes * %6d counts¥n", allocator8_.allocNum());
		kuto_printf("  small alloc : 16 bytes * %6d counts¥n", allocator16_.allocNum());
		kuto_printf("  small alloc : 32 bytes * %6d counts¥n", allocator32_.allocNum());
		kuto_printf("  small alloc : 48 bytes * %6d counts¥n", allocator48_.allocNum());
		kuto_printf("  small alloc : 64 bytes * %6d counts¥n", allocator64_.allocNum());
	}
	
private:
	StaticMemoryAllocator<8, 64>		allocator8_;
	StaticMemoryAllocator<16, 64>		allocator16_;
	StaticMemoryAllocator<32, 64>		allocator32_;
	StaticMemoryAllocator<48, 32>		allocator48_;
	StaticMemoryAllocator<64, 32>		allocator64_;
};


}	// namespace kuto

