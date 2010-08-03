/**
 * @file
 * @brief error
 * @author project.kuto
 */
#pragma once


#if defined(DEBUG)
	#define kuto_printf(...) kuto::debug_printf(__VA_ARGS__)
#else
	#define NDEBUG
	#define kuto_printf(...)
#endif

#include <cassert>
#define kuto_assert(condition) assert(condition)

namespace kuto {

void debug_printf(const char* str, ...);

}	// namespace kuto
