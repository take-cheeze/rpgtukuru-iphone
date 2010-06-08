/**
 * @file
 * @brief Memory Manager
 * @author project.kuto
 */
#pragma once

#include "kuto_small_memory_allocator.h"


namespace kuto {


class Memory
{
public:
	enum AllocType {
		kAllocTypeAlloc,
		kAllocTypeNew,
		kAllocTypeNewArray,
		kAllocTypeMax
	};
	static Memory* instance();

public:
	Memory();
	
	void* alloc(int size) { return allocImpl(kAllocTypeAlloc, size); }
	void dealloc(void* mem) { deallocImpl(kAllocTypeAlloc, mem); }

	void* allocImpl(AllocType type, int size);
	void deallocImpl(AllocType type, void* mem);

	void print();

private:
	// int							totalSize_;
	int							allocSize_[kAllocTypeMax];
	int							allocCount_[kAllocTypeMax];
	SmallMemoryAllocator		smallAllocator_;

	struct MemInfo
	{
		// const char sign[5] = "kuto";
		Memory::AllocType type;
		int size;
	};
};	// class Memory

}	// namespace kuto

