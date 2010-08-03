/**
 * @file
 * @brief コマンドバッファ　いろんなクラスのアロケータ
 * @author project.kuto
 */
#pragma once


namespace kuto {

/// コマンドバッファ
class CommandBuffer
{
public:
	CommandBuffer() : buffer_(NULL), size_(0), currentPosition_(0) {}
	CommandBuffer(u32 size) : buffer_(NULL), size_(0), currentPosition_(0) { createBuffer(size); }
	~CommandBuffer() { destroyBuffer(); }

	void createBuffer(u32 size) { destroyBuffer(); buffer_ = new char[size]; size_ = size; }
	void destroyBuffer() { if (buffer_) delete[] buffer_; buffer_ = NULL; size_ = 0; }
	u32 size() const { return size_; }

	void clear() { currentPosition_ = 0; }
	template<class T> T* alloc() {
		int allocSize = sizeof(T);
		allocSize += 4 - (allocSize & 3);
		if (allocSize + currentPosition_ > size_)
			return NULL;
		T* ret = new (buffer_ + currentPosition_) T();
		currentPosition_ += allocSize;
		return ret;
	}
	template<class T> T* allocArray(int size) {
		int allocSize = sizeof(T) * size + 8;		// 8バイト程度の配列ヘッダが必要
		allocSize += 4 - (allocSize & 3);
		if (allocSize + currentPosition_ > size_)
			return NULL;
		T* ret = new (buffer_ + currentPosition_) T[size];
		currentPosition_ += allocSize;
		return ret;
	}


private:
	char*			buffer_;
	u32				size_;
	u32				currentPosition_;
};	// class CommandBuffer

}	// namespace kuto
