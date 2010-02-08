/**
 * @file
 * @brief error
 * @author project.kuto
 */
#pragma once


#if defined(DEBUG)

#include <assert.h>

namespace kuto {

void debug_printf(const char* str, ...);
	
}	// namespace kuto

#define kuto_assert(condition) assert(condition)
#define kuto_printf(...) kuto::debug_printf(__VA_ARGS__)

#else

#define kuto_assert(condition)
#define kuto_printf(...)

#endif