/**
 * @file
 * @brief Static Buffer String
 * @author takuto
 */
#pragma once

#include "kuto_types.h"
#include "kuto_error.h"


namespace kuto {

template<class T, uint SIZE>
class StaticString
{
public:
	typedef T element_type;
	typedef T* iterator;
	typedef const T* const_iterator;
	
public:
	StaticString() { buffer_[0] = NULL; }
	StaticString(const StaticString& rhs) { assign(rhs); }
	StaticString(const T* buffer) { assign(buffer); }
	
	StaticString& append(const StaticString& rhs) { return append(rhs.c_str()); }
	StaticString& append(const T* buffer) {
		int endIndex = size();
		for (u32 i = 0; i < SIZE; i++) {
			buffer_[i + endIndex] = buffer[i];
			if (!buffer_[i + endIndex])
				break;
			kuto_assert(i < SIZE - 1);
		}
		return *this;
	}
	StaticString& assign(const StaticString& rhs) { return assign(rhs.c_str()); }
	StaticString& assign(const T* buffer) {
		for (u32 i = 0; i < SIZE; i++) {
			buffer_[i] = buffer[i];
			if (!buffer_[i])
				break;
			kuto_assert(i < SIZE - 1);
		}
		return *this;
	}
	T at(u32 index) const { kuto_assert(index < SIZE); return buffer_[index]; }
	T& at(u32 index) { kuto_assert(index < SIZE); return buffer_[index]; }
	int compare(const StaticString& rhs) const { return compare(rhs.c_str()); }
	int compare(const T* buffer) const {
		for (u32 i = 0; i < SIZE; i++) {
			if (buffer_[i] < buffer[i])
				return -1;
			else if (buffer_[i] > buffer[i])
				return 1;
			if (!buffer_[i])
				break;
			kuto_assert(i < SIZE - 1);
		}
		return 0;
	}
	bool empty() const { return buffer_[0] == NULL; }
	u32 size() const { T* top = buffer_; while (*top) ++top; return top - buffer_; }
	u32 capacity() const { return SIZE; }
	const T* c_str() const { return buffer_; }
	iterator begin() { return buffer_; }
	iterator end() { return buffer_ + size(); }
	const_iterator begin() const { return buffer_; }
	const_iterator end() const { return buffer_ + size(); }
	
	T operator[](u32 index) const { return at(index); }
	T& operator[](u32 index) { return at(index); }
	StaticString& operator=(const StaticString& rhs) { return assign(rhs); }
	StaticString& operator=(const T* buffer) { return assign(buffer); }
	StaticString& operator+=(const StaticString& rhs) { return append(rhs); }
	StaticString& operator+=(const T* buffer) { return append(buffer); }
	bool operator==(const StaticString& rhs) const { return compare(rhs) == 0; }
	bool operator==(const T* buffer) const { return compare(buffer) == 0; }
	bool operator!=(const StaticString& rhs) const { return compare(rhs) != 0; }
	bool operator!=(const T* buffer) const { return compare(buffer) != 0; }	
	bool operator<(const StaticString& rhs) const { return compare(rhs) < 0; }
	bool operator<(const T* buffer) const { return compare(buffer) < 0; }	
	bool operator>(const StaticString& rhs) const { return compare(rhs) > 0; }
	bool operator>(const T* buffer) const { return compare(buffer) > 0; }	
	bool operator<=(const StaticString& rhs) const { return compare(rhs) <= 0; }
	bool operator<=(const T* buffer) const { return compare(buffer) <= 0; }	
	bool operator>=(const StaticString& rhs) const { return compare(rhs) >= 0; }
	bool operator>=(const T* buffer) const { return compare(buffer) >= 0; }	

	StaticString operator+(const StaticString& rhs) const { StaticString ret(*this); return ret += rhs; }
	StaticString operator+(const T* buffer) const { StaticString ret(*this); return ret += buffer; }

private:
	T			buffer_[SIZE];
};	// class StaticString


typedef StaticString<char, 8>	StaticString8;
typedef StaticString<char, 16>	StaticString16;
typedef StaticString<char, 32>	StaticString32;
typedef StaticString<char, 64>	StaticString64;
typedef StaticString<char, 128>	StaticString128;
typedef StaticString<char, 256>	StaticString256;

typedef StaticString<wchar_t, 8>	StaticWString8;
typedef StaticString<wchar_t, 16>	StaticWString16;
typedef StaticString<wchar_t, 32>	StaticWString32;
typedef StaticString<wchar_t, 64>	StaticWString64;
typedef StaticString<wchar_t, 128>	StaticWString128;
typedef StaticString<wchar_t, 256>	StaticWString256;


}	// namespace kuto
