/**
 * @file
 * @brief Static Memory Allocator
 * @author project.kuto
 */
#pragma once

#include <climits>

#include "kuto_types.h"
#include "kuto_error.h"
#include "kuto_array.h"

namespace kuto {


// MEM_SIZE * ( CHAR_BIT * sizeof(uint) ) * CHUNK_NUM
template< uint MEM_SIZE, uint CHUNK_NUM >
class StaticMemoryAllocator
{
public:
	typedef uint UseFlag;
private:
	static const uint BUFFER_NUM = CHAR_BIT * sizeof(UseFlag); // 32
	static const UseFlag ALL_USED = ~0;
public:
	struct Chunk {
		UseFlag		used;
		u8			buffer[BUFFER_NUM][MEM_SIZE];

		Chunk() : used(0) {}
	};

public:
	StaticMemoryAllocator() : count_(0) {}

	bool empty() const
	{
		for(uint i = 0; i < CHUNK_NUM; i++) if(chunks_[i].used != 0) return false;
		return true;
	}
	bool resetCountIfEmpty()
	{
		if( empty() ) { count_ = 0; return true; }
		else return false;
	}

	u8* alloc()
	{
		// TODO: multithread things
		if( count_ < (BUFFER_NUM * CHUNK_NUM) ) {
			Chunk& c = chunks_[count_ / BUFFER_NUM];
			int bufNo = (count_ % BUFFER_NUM);
			count_++;
			c.used |= (0x1 << bufNo);
			return c.buffer[bufNo];
		} else for (typename Array< Chunk, CHUNK_NUM >::iterator it = chunks_.begin(); it < chunks_.end(); ++it) {
			if (it->used != ALL_USED)
			for (uint bit = 0; bit < BUFFER_NUM; bit++) {
				if ((it->used & (1 << bit)) == 0) {
					it->used |= (1 << bit);
					return it->buffer[bit];
				}
			}
		}
		// full buffer
		return NULL;
	}

	bool free(void* buffer)
	{
		if (buffer >= chunks_.begin() && buffer < chunks_.end()) {
			uint iArray = ((u8*)buffer - (u8*)chunks_.get()) / sizeof(Chunk);
			kuto_assert(iArray < chunks_.size());
			uint bit = ((u8*)buffer - (u8*)chunks_[iArray].buffer) / MEM_SIZE;
			kuto_assert(bit < BUFFER_NUM);
			// kuto_assert( chunks_[iArray].used & (0x1 << bit) ); // check double free
			chunks_[iArray].used &= ~(1 << bit);
			return true;
		}
		return false; // not a buffer
	}

	int allocNum() const
	{
		int allocNum = 0;
		for (uint iArray = 0; iArray < chunks_.size(); iArray++) {
			if (chunks_[iArray].used != ALL_USED) {
				for (uint i = 0; i < BUFFER_NUM; i++) {
					if ((chunks_[iArray].used & (1 << i)) != 0) {
						allocNum++;
					}
				}
			}
		}
		return allocNum;
	}

private:
	uint count_;
	Array< Chunk, CHUNK_NUM >		chunks_;
}; // class StaticMemoryAllocator


}	// namespace kuto
