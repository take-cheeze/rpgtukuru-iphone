/**
 * @file
 * @brief Memory Manager
 * @author project.kuto
 */
#pragma once


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
	int				totalSize_;
	int				allocSize_[kAllocTypeMax];
	int				allocCount_[kAllocTypeMax];
};	// class Memory

}	// namespace kuto

