/**
 * @file
 * @brief Memory Manager
 * @author project.kuto
 */
#pragma once

#include "kuto_small_memory_allocator.h"
#include "kuto_singleton.h"


namespace kuto {


class Memory : public Singleton<Memory>
{
	friend class Singleton<Memory>;
public:
	enum AllocType {
		kAllocTypeAlloc,
		kAllocTypeNew,
		kAllocTypeNewArray,
		kAllocTypeMax
	};

public:
	void* alloc(int size) { return allocImpl(kAllocTypeAlloc, size); }
	void dealloc(void* mem) { deallocImpl(kAllocTypeAlloc, mem); }

	void* allocImpl(AllocType type, uint size);
	void deallocImpl(AllocType type, void* mem);

	void print();

	void disableSmallAllocator(bool val = true) { disableSmallAllocator_ = val; }

	void resetAllocatorsIfEmpty() { smallAllocator_.resetAllocatorsIfEmpty(); }

protected:
	Memory();

private:
	bool disableSmallAllocator_;
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
