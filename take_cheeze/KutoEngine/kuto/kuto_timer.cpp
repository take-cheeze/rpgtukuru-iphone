/**
 * @file
 * @brief Timer
 * @author project.kuto
 */

#include "kuto_timer.h"
// #include <mach/mach.h>
// #include <mach/mach_time.h>
#include <unistd.h>


namespace kuto {

u64 Timer::getTime()
{
	// return mach_absolute_time();
	return 0;
}

u64 Timer::getElapsedTimeInNanoseconds(u64 startTime, u64 endTime)
{
/*
	u64 elapsed = endTime - startTime;
	static mach_timebase_info_data_t sTimebaseInfo = {0, 0};

	if ( sTimebaseInfo.denom == 0 ) {
		mach_timebase_info(&sTimebaseInfo);
	}
	return elapsed * sTimebaseInfo.numer / sTimebaseInfo.denom;
 */
	return 0;
}

}	// namespace kuto
