/**
 * @file
 * @brief Utilities
 * @author project.kuto
 */
#pragma once

#include "kuto_types.h"
#include <string>
#include <cstdlib>
#include "time.h"

namespace kuto {

const float PI		= 3.141592f;

template<class T> T min(T lhs, T rhs) { return lhs < rhs? lhs : rhs; }
template<class T> T max(T lhs, T rhs) { return lhs > rhs? lhs : rhs; }
template<class T> T clamp(T x, T lower, T upper) { return max(lower, min(upper, x)); }
template<class T> T lerp(T v0, T v1, float ratio) { return (v1 - v0) * ratio + v0; }

template<class T> void safeDelete(T*& value) { if (value) { delete value; value = NULL; } }

std::string sjis2utf8(const std::string& str);
std::string utf82sjis(const std::string& str);

inline void randomize() { srand((u32)time(NULL)); }
inline float random(float max) { return ((float)(rand() % 100000) / 100000.f) * max; }
inline int random(int max) { return rand() % max; }

u32 crc32(const char* data, u32 size);
inline u32 crc32(const std::string& data) { return crc32(data.c_str(), data.size()); }

int fixPowerOfTwo(int value);
	
}

