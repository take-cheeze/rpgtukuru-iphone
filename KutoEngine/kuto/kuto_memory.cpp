/**
 * @file
 * @brief Memory Manager
 * @author project.kuto
 */

#include <cstdlib>

#include "kuto_memory.h"
#include "kuto_error.h"

#include "AppMain.h"

#include <rpg2k/Define.hpp>


#if RPG2K_IS_PSP
	#define kuto_malloc std::malloc
	#define kuto_free std::free
#elif 0
	#define ABORT_ON_ASSERT_FAILURE 0
	#include "nedmalloc/nedmalloc.c"
	#define kuto_malloc nedalloc::nedmalloc
	#define kuto_free nedalloc::nedfree
#else
	#define USE_LOCKS 1			// iPhoneだとどうも別スレッドでUIとか色々動いているようで。。。
	#define USE_DL_PREFIX
	#include "malloc.c"
	#define kuto_malloc dlmalloc
	#define kuto_free dlfree
#endif

namespace kuto {

Memory::Memory()
: disableSmallAllocator_(false)
{
	std::memset(allocSize_, 0, sizeof(allocSize_));
	std::memset(allocCount_, 0, sizeof(allocCount_));
}

void* Memory::allocImpl(AllocType type, uint size)
{
	u8* ret = NULL;
	if (
		!disableSmallAllocator_ &&
		( size <= smallAllocator_.maxAllocSize() ) &&
		( ret = smallAllocator_.alloc(size) )
	) return ret;

	allocSize_[type] += size;
	allocCount_[type]++;

	ret = reinterpret_cast<u8*>(kuto_malloc(size + sizeof(MemInfo)));
	kuto_assert(ret);
	MemInfo* info = reinterpret_cast<MemInfo*>(ret);
	info->type = type;
	info->size = size;
	return ret + sizeof(MemInfo);
}

void Memory::deallocImpl(AllocType type, void* mem)
{
	if (smallAllocator_.free(mem)) {
		return;
	}

	u8* realMem = reinterpret_cast<u8*>(mem) - sizeof(MemInfo);
	MemInfo* info = reinterpret_cast<MemInfo*>(realMem);

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

	#ifdef USE_DL_PREFIX
		dlmalloc_stats();
	#endif

	smallAllocator_.print();
}

}	// namespace kuto



void* operator new(size_t size) throw (std::bad_alloc)
{
	void* ret = kuto::Memory::instance().allocImpl(kuto::Memory::kAllocTypeNew, size);
	if(ret == NULL) throw std::bad_alloc();
	else return ret;
}

void operator delete(void* mem) throw()
{
	if(mem)
		kuto::Memory::instance().deallocImpl(kuto::Memory::kAllocTypeNew, mem);
}

void* operator new[](size_t size) throw (std::bad_alloc)
{
	void* ret = kuto::Memory::instance().allocImpl(kuto::Memory::kAllocTypeNewArray, size);
	if(ret == NULL) throw std::bad_alloc();
	else return ret;
}

void operator delete[](void* mem) throw()
{
	if(mem)
		kuto::Memory::instance().deallocImpl(kuto::Memory::kAllocTypeNewArray, mem);
}
