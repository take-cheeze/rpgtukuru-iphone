/**
 * @file
 * @brief Static Memory Allocator
 * @author project.kuto
 */
#pragma once

#include <list>
#include "kuto_types.h"
#include "kuto_error.h"
#include "kuto_array.h"

namespace kuto {

template<int MEM_SIZE, int ARRAY_SIZE>
class StaticMemoryAllocator
{
public:
	struct Chunk {
		u32		used;
		char	buffer[32][MEM_SIZE];
		
		Chunk() : used(0) {}
	};
	
public:
	char* alloc() {
		for (u32 iArray = 0; iArray < chunks_.size(); iArray++) {
			if (chunks_[iArray].used != 0xFFFFFFFF) {
				for (int i = 0; i < 32; i++) {
					if ((chunks_[iArray].used & (1 << i)) == 0) {
						chunks_[iArray].used |= (1 << i);
						return chunks_[iArray].buffer[i];
					}
				}
			}
		}
		// full buffer
		return NULL;
	}
	
	bool free(void* buffer) {
		if (buffer >= chunks_.begin() && buffer < chunks_.end()) {
			int iArray = (int)((char*)buffer - (char*)chunks_.get()) / sizeof(Chunk);
			kuto_assert(iArray >= 0 && iArray < chunks_.size());
			int i = (int)((char*)buffer - (char*)chunks_[iArray].buffer) / MEM_SIZE;
			kuto_assert(i >= 0 && i < 32);
			chunks_[iArray].used &= ~(1 << i);
			return true;
		}
		// no buffer
		return false;
	}
	
	int allocNum() const {
		int allocNum = 0;
		for (u32 iArray = 0; iArray < chunks_.size(); iArray++) {
			if (chunks_[iArray].used != 0xFFFFFFFF) {
				for (int i = 0; i < 32; i++) {
					if ((chunks_[iArray].used & (1 << i)) != 0) {
						allocNum++;
					}
				}
			}
		}
		return allocNum;
	}
	
private:
	Array<Chunk, ARRAY_SIZE>		chunks_;
};


}	// namespace kuto

