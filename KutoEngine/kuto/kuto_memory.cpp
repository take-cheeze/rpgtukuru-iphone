/**
 * @file
 * @brief Memory Manager
 * @author project.kuto
 */

#include <cstdio>
#include <cstdlib>
#include "kuto_memory.h"
#include "kuto_error.h"


namespace kuto {

Memory* Memory::instance()
{
	static Memory sMemory;
	return &sMemory;
}

Memory::Memory()
// : totalSize_(0)
{
	std::memset(allocSize_, 0, sizeof(allocSize_));
	std::memset(allocCount_, 0, sizeof(allocCount_));
}

void* Memory::allocImpl(AllocType type, int size)
{
	char* mem = NULL;
	if ( (uint)size <= smallAllocator_.maxAllocSize()) {
		mem = smallAllocator_.alloc(size);
	}
	if (mem)
		return mem;
	// totalSize_ += size;
	allocSize_[type] += size;
	allocCount_[type]++;

	mem = reinterpret_cast< char* >(std::malloc(size + sizeof(MemInfo)));
	MemInfo* info = reinterpret_cast< MemInfo* >(mem);
	info->type = type;
	info->size = size;
	return mem + sizeof(MemInfo);
}

void Memory::deallocImpl(AllocType type, void* mem)
{
	if (smallAllocator_.free(mem))
		return;

	u8* realMem = reinterpret_cast< u8* >(mem) - sizeof(MemInfo);
	MemInfo* info = reinterpret_cast< MemInfo* >(realMem);
	kuto_assert(type == info->type);
	int size = info->size;
	// totalSize_ -= size;
	allocSize_[type] -= size;
	allocCount_[type]--;
	std::free(realMem);
}

void Memory::print()
{
	int totalSize = 0;
	for(int i = 0; i < kAllocTypeMax; i++) totalSize += allocSize_[i];
	kuto_printf("total   : %8d bytes¥n", totalSize);// totalSize_);
	kuto_printf("  alloc : %8d bytes / %6d counts¥n", allocSize_[kAllocTypeAlloc], allocCount_[kAllocTypeAlloc]);
	kuto_printf("  new   : %8d bytes / %6d counts¥n", allocSize_[kAllocTypeNew], allocCount_[kAllocTypeNew]);
	kuto_printf("  new[] : %8d bytes / %6d counts¥n", allocSize_[kAllocTypeNewArray], allocCount_[kAllocTypeNewArray]);
	smallAllocator_.print();
}

}	// namespace kuto



void* operator new(size_t size)
{
	return kuto::Memory::instance()->allocImpl(kuto::Memory::kAllocTypeNew, size);
}

void operator delete(void* mem)
{
	kuto::Memory::instance()->deallocImpl(kuto::Memory::kAllocTypeNew, mem);
}

void* operator new[](size_t size)
{
	return kuto::Memory::instance()->allocImpl(kuto::Memory::kAllocTypeNewArray, size);
}

void operator delete[](void* mem)
{
	kuto::Memory::instance()->deallocImpl(kuto::Memory::kAllocTypeNewArray, mem);
}
