/**
 * @file
 * @brief Memory Manager
 * @author project.kuto
 */

#include <cstdio>
#include <cstdlib>
#include "kuto_memory.h"
#include "kuto_error.h"


#if (RPG2K_IS_WINDOWS || RPG2K_IS_PSP)
	#define kuto_malloc std::malloc
	#define kuto_free std::free
#else
	#define USE_LOCKS 1			// iPhoneだとどうも別スレッドでUIとか色々動いているようで。。。
	#define USE_DL_PREFIX
	#include "malloc.c"
	#define kuto_malloc dlmalloc
	#define kuto_free dlfree
#endif

namespace kuto {

Memory* Memory::instance()
{
	static Memory sMemory;
	return &sMemory;
}

Memory::Memory()
{
	std::memset(allocSize_, 0, sizeof(allocSize_));
	std::memset(allocCount_, 0, sizeof(allocCount_));
}

void* Memory::allocImpl(AllocType type, uint size)
{
	u8* mem = NULL;
	if ( size <= smallAllocator_.maxAllocSize() ) {
		if( ( mem = smallAllocator_.alloc(size) ) ) return mem;
	}

	allocSize_[type] += size;
	allocCount_[type]++;

	mem = reinterpret_cast< u8* >(kuto_malloc(size + sizeof(MemInfo)));
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
	allocSize_[type] -= size;
	allocCount_[type]--;
	kuto_free(realMem);
}

void Memory::print()
{
	int totalSize = 0;
	for(int i = 0; i < kAllocTypeMax; i++) totalSize += allocSize_[i];
	kuto_printf("total   : %8d bytes¥n", totalSize);

	kuto_printf("  alloc : %8d bytes / %6d counts¥n", allocSize_[kAllocTypeAlloc], allocCount_[kAllocTypeAlloc]);
	kuto_printf("  new   : %8d bytes / %6d counts¥n", allocSize_[kAllocTypeNew], allocCount_[kAllocTypeNew]);
	kuto_printf("  new[] : %8d bytes / %6d counts¥n", allocSize_[kAllocTypeNewArray], allocCount_[kAllocTypeNewArray]);
	smallAllocator_.print();
}

}	// namespace kuto



void* operator new(size_t size) throw (std::bad_alloc)
{
	return kuto::Memory::instance()->allocImpl(kuto::Memory::kAllocTypeNew, size);
}

void operator delete(void* mem) throw()
{
	kuto::Memory::instance()->deallocImpl(kuto::Memory::kAllocTypeNew, mem);
}

void* operator new[](size_t size) throw (std::bad_alloc)
{
	return kuto::Memory::instance()->allocImpl(kuto::Memory::kAllocTypeNewArray, size);
}

void operator delete[](void* mem) throw()
{
	kuto::Memory::instance()->deallocImpl(kuto::Memory::kAllocTypeNewArray, mem);
}
