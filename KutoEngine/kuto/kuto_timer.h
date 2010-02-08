/**
 * @file
 * @brief Timer
 * @author project.kuto
 */
#pragma once

#include "kuto_types.h"


namespace kuto {

class Timer
{
public:
	static u64 getTime();
	static u64 getElapsedTimeInNanoseconds(u64 startTime, u64 endTime);
	static u64 getFPS(u64 startTime, u64 endTime);
};	// class Timer

}	// namespace kuto
