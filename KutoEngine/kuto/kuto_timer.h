/**
 * @file
 * @brief Timer
 * @author project.kuto
 */
#pragma once

#include "kuto_types.h"

#include <utility>


namespace kuto {

class Timer
{
public:
	static u64 time();
	static u64 elapsedTimeInNanoseconds(u64 startTime, u64 endTime);
	static u64 elapsedTimeInNanoseconds(std::pair<u64, u64> time)
	{
		return elapsedTimeInNanoseconds(time.first, time.second);
	}
	static u64 getFPS(u64 startTime, u64 endTime);
};	// class Timer

}	// namespace kuto
