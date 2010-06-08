/**
 * @file
 * @brief types
 * @author project.kuto
 */
#pragma once

/*
 * 2010/06/08: vc2010's <stdint.h> has a bug so be careful
 */
#include <stdint.h>

#include <cmath>
#include <iostream>

#include "kuto_define.h"

namespace kuto
{
	typedef int8_t  s8 ;
	typedef int16_t s16;
	typedef int32_t s32;
	typedef int64_t s64;

	typedef uint8_t  u8 ;
	typedef uint16_t u16;
	typedef uint32_t u32;
	typedef uint64_t u64;
/*
	typedef char s8;
	typedef short s16;
	typedef int s32;
	typedef long long int s64;

	typedef unsigned char u8;
	typedef unsigned short u16;
	typedef uint u32;
	typedef unsigned long long int u64;
 */

	typedef float f32;
	typedef double f64;

}
typedef unsigned int uint;
